#include "EventProcessor.hpp"

eventProcessor::eventProcessor(eventManager& em, connectionManager& cm, int serverFd) : _eventManager(em), _connectionManager(cm) _serverFd(serverFd) {
	_requestProcessor = NULL; //todo
	_shouldStop = false;
}

eventProcessor::~eventProcessor();

//private helper method
void eventProcessor::acceptNewConnections() {
	
}
void eventProcessor::processIncomingData(int clientFd, const WebServConfig& config) {
	//handle what to do with received data. Is it a CGI ? An chunked request or a basic get ?
}

void eventProcessor::handleReceiveError(int clientFd, ssize_t recvResult) {

}

void eventProcessor::sendResponse(int clientFd, const std::string response) {
	//send final response
}

//public events handler
void eventProcessor::runEventLoop(const Webservconfig& config) {
	while (!_shouldStop) {
		int nfds = _eventManager.wait(-1);
		if (nfds < 0) {/*error*/}
		for (int i = 0; i < nfds; ++i) {
			int fd = _ev[i].data.fd;
			uint32_t events = _ev[i].events;
			if (isServerSocket(fd)) {
				handleServerEvents(fd);
			}
			else if (isDisconnectionEvent(events)){
				handleClientDisconnection(fd);
			}
			else if (isDataReadyEvent(events)) {
				handleClientData(fd, config);
			}
		}
	}
}

void eventProcessor::stopEventLoop() {
	if (_shouldStop == true) {
	}
}

void eventProcessor::handleServerEvents(int serverFd) {
	acceptNewConnections();
	//handle onConn ?
}
void eventProcessor::handleClientDisconnection(int clientFd) {
	_connectionManager.removeConnections(clientFd);
	//EPOLLUP | EPOLLERR | EPOLLRDUP
} 

void eventProcessor::handleClientData(int serverFd) {
	//on data ready
}

bool eventProcessor::isServerSocket(int fd) const {
	if (fd == _serverFd)
		return true;
	return false;
}
bool eventProcessor::isDisconnectionEvent(uint32_t event) const {
	if (event && (EPOLLHUP | EPOLLERR | EPOLLRDHUP))
		return true;
	return false;
}
bool eventProcessor::isDataReadyEvent(uint32_t event) const {
	if (event == EPOLLIN)
		return true;
	return false;
}
