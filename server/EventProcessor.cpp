#include "EventProcessor.hpp"

eventProcessor::eventProcessor(eventManager& em, connectionManager& cm, const std::vector<int>& serverSocket) : _eventManager(em), _connectionManager(cm), _serverSocket(serverSocket), _shouldStop(false) {
}

eventProcessor::~eventProcessor() {}


void eventProcessor::acceptNewConnections(int serverFd) {
	while (true) {
		struct sockaddr_storage clientAddr;
		int clientFd = NetworkHandler::acceptConnection(serverFd, clientAddr);
		if (clientFd < 0)
			break;
		_eventManager.addFd(clientFd, EPOLLIN | EPOLLRDHUP);
		_connectionManager.addConnection(clientFd, serverFd);
		console::log("New client to server on FD:", clientFd, SRV);
	}
}

void eventProcessor::handleReceiveError(int clientFd, ssize_t recvResult) {
	if (recvResult == 0) {
		console::log("Client closed the connection on FD: ", clientFd, SRV);
		handleClientDisconnection(clientFd);
	}
	if (recvResult == -1){
		console::log("Real error, closing connection on FD: ", clientFd, SRV);
		handleClientDisconnection(clientFd);
	}
}

void eventProcessor::sendResponse(int clientFd, const std::string& response) {
	Conn& connection = _connectionManager.getConnection(clientFd);
	onConn::updateActivity(connection);
	ssize_t bytesSent = NetworkHandler::sendData(clientFd, const_cast<char*>(response.data()), response.size());
	if (bytesSent < 0) {
		console::log("Failed to send responses on FD: ", clientFd, SRV);
		handleClientDisconnection(clientFd);
		return ;
	}
	if (bytesSent == (ssize_t)response.size()) {
		console::log("Send full response on FD: ", clientFd, SRV);
		_eventManager.modFd(clientFd, EPOLLIN | EPOLLRDHUP);
	}
	else if (bytesSent > 0) {
		connection.outBuffer = response;
		connection.outSent = bytesSent;
		connection.hasDataToSend = true;

		_eventManager.modFd(clientFd, EPOLLIN | EPOLLOUT | EPOLLRDHUP);
		console::log("Partial send, waiting for EPOLLOUT on FD: ", clientFd, SRV);
	}
	else {
		console::log("Send error, closing FD", clientFd, SRV);
		handleClientDisconnection(clientFd);
	}
}

void eventProcessor::handleClientWriteReady(int clientFd) {
	Conn& connection = _connectionManager.getConnection(clientFd);
	onConn::updateActivity(connection);
	if (!connection.hasDataToSend) {
		_eventManager.modFd(clientFd, EPOLLIN | EPOLLRDHUP);
		return ;
	}
	const char* dataToSend = connection.outBuffer.data() + connection.outSent;
	size_t remainingBytes = connection.outBuffer.size() - connection.outSent;
	ssize_t bytesSent = NetworkHandler::sendData(clientFd, const_cast<char*>(dataToSend), remainingBytes);
	if (bytesSent > 0) {
		connection.outSent += bytesSent;
		if (connection.outSent >= connection.outBuffer.size()) {
			connection.hasDataToSend = false;
			connection.outBuffer.clear();
			connection.outSent = 0;
			_eventManager.modFd(clientFd, EPOLLIN | EPOLLRDHUP);
		}
	}
	else {
		console::log("Send error during EPOLLOUT on FD: ", clientFd, SRV);
		handleClientDisconnection(clientFd);
	}
}

void eventProcessor::runEventLoop(const WebservConfig& config) {
	time_t lastTimeOutCheck = time(NULL);
	const int timeOutInterval = ServerConstants::TIMEOUT_CHECK;

	while (!server::getShutDownRequest()) {
		int nfds = _eventManager.wait(5000);
		if (nfds < 0) {
			if (errno == EINTR) continue;
			console::log("epoll_wait", std::strerror(errno), ERROR);
			_shouldStop = true;
			break ;
		}
		for (int i = 0; i < nfds; ++i) {
			int fd = _eventManager[i].data.fd;
			uint32_t events = _eventManager[i].events;
			if (server::getShutDownRequest()) {
				break;
			}
			else if (isServerSocket(fd)) {
				acceptNewConnections(fd);
			}
			else if (isCgiPipe(fd)){
				handleCgiData(fd);
			}
			else if (isDisconnectionEvent(events)) {
				handleClientDisconnection(fd);
			}
			else if (isDataReadyEvent(events)) {
				handleClientData(fd, config);
			}
			else if (isDataSendEvent(events)) {
				handleClientWriteReady(fd);
			}
		}
		time_t currentTime = time(NULL);
		if ((currentTime - lastTimeOutCheck) >= timeOutInterval) {
			checkAndCleanTimeout();
			lastTimeOutCheck = currentTime;
		}
		else if (server::getShutDownRequest()) {
			_connectionManager.removeAllConnection();
			return ;
		}
	}
}

void eventProcessor::checkAndCleanTimeout() {
	time_t currentTime = time(NULL);
	std::vector<int> timeOutFds = _connectionManager.getTimedOutConnection(currentTime);
	for (size_t i = 0; i < timeOutFds.size(); ++i) {
		int fd = timeOutFds[i];
		sendTimeOutResponse(fd);
		handleClientDisconnection(fd);
	}
}

void eventProcessor::stopEventLoop() {
	_shouldStop = true;
	console::log("Event stop loop requested", SRV);
}

void eventProcessor::handleClientDisconnection(int clientFd) {
	_connectionManager.removeConnection(clientFd);
	//EPOLLUP | EPOLLERR | EPOLLRDUP
}

void eventProcessor::handleClientData(int clientFd, const WebservConfig& config) {
	Conn& connection = _connectionManager.getConnection(clientFd);
	onConn::updateActivity(connection);

	static const size_t BUFFER_SIZE = ServerConstants::BUFFER_SIZE;

	char buffer[BUFFER_SIZE];
	ssize_t bytesRead = NetworkHandler::receiveData(clientFd, buffer, sizeof(buffer));
	if (bytesRead <= 0) {
		handleReceiveError(clientFd, bytesRead);
		return ;
	}

	connection.in.append(buffer, static_cast<size_t>(bytesRead));
	size_t requestEndPos;
	if (onConn::update_and_ready(connection, requestEndPos)) {
		std::string completeRequest = connection.in.substr(0, requestEndPos);
		std::string response;
		response = handle_messages(config, completeRequest, connection.clientPort, this, clientFd);
		if (!response.empty()){
			sendResponse(clientFd, response);
		}
		else {
			//empty response cgi in court
			connection.waitingForCgi = true;
			console::log("[CGI] Client " + nb_to_string(clientFd) + " waiiting for cgi", MSG);
		}
		connection.in.erase(0, requestEndPos);
	}
	bool alive = true;
	size_t endpos = 0;
	onConn::onDiscon(connection, alive, endpos);
	if (!alive && !connection.waitingForCgi) {
		handleClientDisconnection(clientFd);
	}
}

bool eventProcessor::isServerSocket(int fd) const {
	if (std::find(_serverSocket.begin(), _serverSocket.end(), fd) != _serverSocket.end())
		return true;
	return false;
}

bool eventProcessor::isDisconnectionEvent(uint32_t event) const {
	if (event & (EPOLLHUP | EPOLLERR | EPOLLRDHUP))
		return true;
	return false;
}

bool eventProcessor::isDataReadyEvent(uint32_t event) const {
	if (event & EPOLLIN)
		return true;
	return false;
}

bool eventProcessor::isDataSendEvent(uint32_t event) const {
	if (event & EPOLLOUT)
		return true;
	return false;
}

static std::string build_timeout_response() {
    std::string response =
        "HTTP/1.0 408 Request Timeout\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 147\r\n"
        "Connection: close\r\n"
        "\r\n"
        "<html>\r\n"
        "<head><title>408 Request Timeout</title></head>\r\n"
        "<body>\r\n"
        "<h1>Request Timeout</h1>\r\n"
        "<p>The server timed out waiting for the request.</p>\r\n"
        "</body>\r\n"
        "</html>\r\n";
    return response;
}

void eventProcessor::sendTimeOutResponse(int clientFd) {
	std::string timeOutResp = build_timeout_response();
	console::log("[TIMEOUT] timeout on client:", clientFd, SRV);
	ssize_t bytesSent = NetworkHandler::sendData(clientFd, const_cast<char*>(timeOutResp.data()), timeOutResp.size());
	if (bytesSent < 0) {
		console::log("Failed to send responses on FD: ", clientFd, SRV);
		handleClientDisconnection(clientFd);
		return ;
	}
}

void eventProcessor::handleCgiStart(int clientFd, const CgiResult& cgi){
	if (!cgi.success) return;

	CgiState state;
	state.pipefd = cgi.pipeFd;
	state.pid = cgi.pid;
	state.startTime = time(NULL);
	state.output = "";
	state.clientFd = clientFd;

	_runningCgi[cgi.pipeFd] = state;
	_eventManager.addFd(cgi.pipeFd, EPOLLIN);
}

void eventProcessor::handleCgiData(int pipeFd){

	std::map<int, CgiState>::iterator it = _runningCgi.find(pipeFd);
	if (it == _runningCgi.end()) {
		return;
	}


	CgiState& cgi = it->second;
	char buffer[4096];
	ssize_t bytes = read(pipeFd, buffer, sizeof(buffer));

	if (bytes > 0){
		cgi.output.append(buffer, bytes);
	}
	else if (bytes == 0){
		int status;
		pid_t result = waitpid(cgi.pid, &status, WNOHANG);
		if (result <= 0){
			return;
		}
		if (!cgi.output.empty()){
			std::string response = cgi.output;
			if (response.find("HTTP/") != 0){
				response = "HTTP/1.1 200 OK\r\n" + response;
			}
			sendResponse(cgi.clientFd, response);

			Conn& connection = _connectionManager.getConnection(cgi.clientFd);
			connection.waitingForCgi = false;

			handleClientDisconnection(cgi.clientFd);
		}
		else{
			console::log("[CGI] No output, sending error", ERROR);
			std::string error = "HTTP/1.0 500 Internal Server Error \r\n\r\n<h1>CGI Error</h1>";
			sendResponse(cgi.clientFd, error);
		}
		_eventManager.delFd(pipeFd);
		close(pipeFd);
		_runningCgi.erase(it);
	}
	else{
		console::log("[CGI] read() returned -1 (EAGAIN or error)", ERROR);
	}
}

bool eventProcessor::isCgiPipe(int fd) const{
	bool result = _runningCgi.find(fd) != _runningCgi.end();
	return _runningCgi.find(fd) != _runningCgi.end();
}
