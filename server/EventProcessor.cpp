#include "EventProcessor.hpp"

// 1. Generate 100KB response
// 2. send() → 30KB sent, 70KB remaining
// 3. Add EPOLLOUT to epoll
// 4. EPOLLOUT event → socket ready
// 5. send() → 50KB sent, 20KB remaining  
// 6. EPOLLOUT event → socket ready
// 7. send() → 20KB sent, 0KB remaining
// 8. Remove EPOLLOUT from epoll
// 9. Close connection

eventProcessor::eventProcessor(eventManager& em, connectionManager& cm, int serverFd) : _eventManager(em), _connectionManager(cm), _serverFd(serverFd), _shouldStop(false) {
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
	ssize_t bytesSent = NetworkHandler::sendFullData(clientFd, response);
	if (bytesSent < 0) {
		console::log("Failed to send responses on FD: ", clientFd, SRV);
		handleClientDisconnection(clientFd);
		return ;
	}
	if (bytesSent == (ssize_t)response.size()) {
		console::log("Send full response on FD: ", clientFd, SRV);
		handleClientDisconnection(clientFd);
	}
	else if (bytesSent > 0) {
		connection.outBuffer = response;
		connection.outSent = bytesSent;
		connection.hasDataToSend = true;

		_eventManager.modFd(clientFd, EPOLLIN | EPOLLOUT | EPOLLRDHUP);
		console::log("Partial send, waiting for EPOLLOUT on FD: ", clientFd, SRV);
	}
	else if (bytesSent == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
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
	if (!connection.hasDataToSend) {
		_eventManager.modFd(clientFd, EPOLLIN | EPOLLRDHUP);
		return ;
	}
	const char* dataToSend = connection.outBuffer.data() + connection.outSent;
	size_t remainingBytes = connection.outBuffer.size() - connection.outSent;
	ssize_t bytesSent = NetworkHandler::sendData(clientFd, dataToSend, remainingBytes);
	if (bytesSent > 0) {
		connection.outSent += bytesSent;
		if (connection.outSent >= connection.outBuffer.size()) {
			connection.hasDataToSend = false;
			connection.outBuffer.clear();
			connection.outSent = 0;
			_eventManager.modFd(clientFd, EPOLLIN | EPOLLRDHUP);
			handleClientDisconnection(clientFd);
		}
	}
	else if (bytesSent == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
		return ; //not ready to send keep waiting for EPOLLOUT
	}
	else {
		console::log("Send error during EPOLLOUT on FD: ", clientFd, SRV);
		handleClientDisconnection(clientFd);
	}
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
			else if (isDataSendEvent(events))
				handleClientWriteReady(fd);
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
	if (!alive) {
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

bool eventProcessor::isDataSendEvent(uint32_t event) const {
	if (event & EPOLLOUT)
		return true;
	return false;
}
