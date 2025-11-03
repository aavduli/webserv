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
	if (recvResult == -1) {
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return ;
		if (errno == EINTR) {
			console::log("Recv got interrupted, closing connection on FD: ", clientFd, SRV);
			handleClientDisconnection(clientFd);
		}
		else {
			console::log("Real error, closing connection on FD: ", clientFd, SRV);
			handleClientDisconnection(clientFd);
		}
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
	else if (errno == EAGAIN || errno == EWOULDBLOCK) {
		connection.outBuffer = response;
		connection.outSent = 0;
		connection.hasDataToSend = true;
		_eventManager.modFd(clientFd, EPOLLIN | EPOLLOUT | EPOLLRDHUP);
		console::log("Send would block, waiting EPOLLOUT on FD: ", clientFd, SRV);
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
	else if (bytesSent == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
		return ;
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
} 

void eventProcessor::handleClientData(int clientFd, const WebservConfig& config) {
	Conn& connection = _connectionManager.getConnection(clientFd); 
	onConn::updateActivity(connection);
	
	static const size_t MAX_REQUEST_SIZE = ServerConstants::MAX_REQUEST_SIZE;
	static const size_t BUFFER_SIZE = ServerConstants::BUFFER_SIZE;
	
	char buffer[BUFFER_SIZE];
	ssize_t bytesRead = NetworkHandler::receiveData(clientFd, buffer, sizeof(buffer));
	if (bytesRead <= 0) {
		handleReceiveError(clientFd, bytesRead);
		return ;
	}
	
	if (connection.in.size() + static_cast<size_t>(bytesRead) > MAX_REQUEST_SIZE) {
		console::log("Request too large, closing connection on FD: ", clientFd, SRV);
		handleClientDisconnection(clientFd);
		return ;
	}
	
	connection.in.append(buffer, static_cast<size_t>(bytesRead));
	size_t requestEndPos;
	if (onConn::update_and_ready(connection, requestEndPos)) {
		std::string completeRequest = connection.in.substr(0, requestEndPos);
		std::string response;
		response = handle_messages(config, completeRequest, connection.clientPort);
		sendResponse(clientFd, response);
		connection.in.erase(0, requestEndPos);
	}
	bool alive = true;
	size_t endpos = 0;
	onConn::onDiscon(connection, alive, endpos);
	if (!alive) {
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
