#include "EventProcessor.hpp"

eventProcessor::eventProcessor(eventManager& em, connectionManager& cm, int serverFd) : _eventManager(em), _connectionManager(cm), _serverFd(serverFd) {
	_requestProcessor = NULL; //todo
	_shouldStop = false;
}

eventProcessor::~eventProcessor() {}

//private helper method
void eventProcessor::acceptNewConnections() {
	while (true) {
		struct sockaddr_storage clientAddr;
		int clientFd = NetworkHandler::acceptConnection(_serverFd, clientAddr);
		if (clientFd < 0)
			break;
		_eventManager.addFd(clientFd, EPOLLIN | EPOLLRDHUP);
		_connectionManager.addConnection(clientFd);
		console::log("New client on server:", clientFd, SRV);
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
			console::log("Recv got interupted, closing connection on FD: ", clientFd, SRV);
			handleClientDisconnection(clientFd);
		}
		else {
			console::log("Real error, closing connection on FD: ", clientFd, SRV);
		}
	}
}

void eventProcessor::sendResponse(int clientFd, const std::string response) {
	ssize_t bytesSent = NetworkHandler::sendFullData(clientFd, response);
	if (bytesSent < 0) {
		console::log("Failed to send responses on FD: ", clientFd, SRV)''
		handleClientDisconnection(clientFd);
		return ;
	}
	console::log("Response sent successfuly on FD: ", clientFd, SRV);
	handleClientDisconnection(clientFd); //not keep alive for the moment
}

//public events handler
void eventProcessor::runEventLoop(const WebservConfig& config) {
	while (!_shouldStop) {
		int nfds = _eventManager.wait(-1);
		if (nfds < 0) {
			if (errno == EINTR) continue;
			console::log("epoll_wait", std::strerror(errno), ERROR);
			_shouldStop = true;
			break ;
		}
		for (int i = 0; i < nfds; ++i) {
			int fd = _eventManager[i].data.fd;
			uint32_t events = _eventManager[i].events;
			if (isServerSocket(fd)) {
				handleServerEvents(fd);
			}
			else if (isDisconnectionEvent(events)) {
				handleClientDisconnection(fd);
			}
			else if (isDataReadyEvent(events)) {
				handleClientData(fd, config);
			}
		}
	}
}

void eventProcessor::stopEventLoop() {
	_shouldStop = true;
	console::log("Event stop loop requested", SRV);
}

void eventProcessor::handleServerEvents(int serverFd) {
	acceptNewConnections();
}
void eventProcessor::handleClientDisconnection(int clientFd) {
	_connectionManager.removeConnection(clientFd);
	//EPOLLUP | EPOLLERR | EPOLLRDUP
} 

void eventProcessor::handleClientData(int clientFd, const WebservConfig& config) {
	Conn& connection = _connectionManager.getConnection(clientFd); 
	char buffer[8192];
	ssize_t bytesRead = NetworkHandler::receiveData(clientFd, buffer, sizeof(buffer));
	if (bytesRead <= 0) {
		handleReceiveError(clientFd, bytesRead);
		return ;
	}
	connection.in.append(buffer, static_cast<size_t>(bytesRead));
	size_t requestEndPos;
	if (onConn::update_and_ready(connection, requestEndPos)) {
		std::string completeRequest = connection.in.substr(0, requestEndPos);
		std::string response = handle_request(config, completeRequest);
		sendResponse(clientFd, response);
		connection.in.erase(0, requestEndPos);
	}
	bool alive = true;
	size_t endpos = 0;
	onConn::onDiscon(connection, alive, endpos);
	if(!alive) {
		handleClientDisconnection(clientFd);
	}
}

bool eventProcessor::isServerSocket(int fd) const {
	if (fd == _serverFd)
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
