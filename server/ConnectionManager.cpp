#include "ConnectionManager.hpp"

connectionManager::connectionManager() : _maxConn(1000), _eventManager(???) {}

connectionManager::connectionManager(eventManager& em, size_t maxConn) :_maxConn(maxConn), _eventManager(em) {
	console::log("ConnectionManager initialized with max connection: " + std::to_string(maxConn), SRV);
}

connectionManager::~connectionManager() {}

bool connectionManager::addConnection(int clientFD) {
	if (_connections.size() >=  _maxConn) {
		console::log("Max connection has been reached", SRV);
		return false;
	}
	if (_connections.find(clientFD) != connections.end()) {
		console::log("Connection already exists for FD: " + std::to_string(clientFD));
		return false;
	}
	_connections[clientFD] = Conn();
	console::log("Add connection for FD: " + std::to_string(clientFD), SRV);
	return true;
}

void connectionManager::removeConnection(int clientFD) {
	 std::map<int, Conn>::iterator it = _connections.find(clientFD);
	 if (it == _connections.end()) {
		console::log("Attempted to remove non existent connection: " + std::to_string(clientFD), ERROR);
		return ;
	 }
	 console::log("Removed connection: " + std::to_string(clientFD), SRV);
	 _eventManager.delFd(clientFD);
	 NetworkHandler::closeConnection(clientFD);
}

bool connectionManager::hasConnection(int clientFD) const {
	return _connections.find(clientFD) != _connections.end();
}

size_t connectionManager::getConnectionCount() {
	return _maxConn;
}

Conn& getConnection(int clientFD) {
	std::map<int, Conn>::iterator it = _connections.find(clientFD);
	if (it == _connections.end()) {
		throw std::runtime_error("Connection not found for fd: " + std::to_string(clientFD));
	}
	return it->second;
}

const Conn& getConnection(const int clientFD) {
	std::map<int, Conn>::iterator it = _connections.find(clientFD);
	if (it == _connections.end()) {
		thr ow std::runtime_error("Connection not found for fd: " + std::to_string(clientFD));
	}
	return it->second;
}
void removeAllConnection() {
	console::log("Removing all connection: " + std::to_string(_connections.size()), SRV);
	for (std::map<int, Conn>::pair : _connections) {
		int fd = pair.first;
		_eventManager.delFD(fd);
		NetworkHandler::closeConnection(fd);
	}
	_connections.clear();
}

std::vector<int> connectionManager::getConnectionFds() const {
	std::vector<int> fds;
	fds.reserve(_connections.size());
	for(std::map<int, Conn>::iterator it; it < _connections.size(); ++it) {
		fds.push_back(it->first);
	}
	return fds;
}

bool connectionManager::isConnectionValid(clientFD) const {
	if (!hasConnection(clientFD)) return false;
	return NetworkHandler::isSocketError(clientFD) == false;
}

void connectionManager::cleanUpStaleConn() {
	std::vector<int> staleConn;
	for (std::map<int, Conn>::iterator it = _connections.begin(); it < _connections.size(); ++it) {
		int fd = it->first;
		if (NetworkHandler::isSocketError(fd))
			staleConn.push_back(fd);
	}
	for (int fd : staleConn) {
		console::log("Removing stale connection: " + std::to_string(fd), SRV);
		removeConnection(fd);
	}
}

void connectionManager::printConnectionStats() const {
    console::log("=== Connection Statistics ===", SRV);
    console::log("Active connections: " + std::to_string(_connections.size()), SRV);
    console::log("Max connections: " + std::to_string(_maxConn), SRV);
    console::log("Capacity used: " + std::to_string((_connections.size() * 100) / _maxConn) + "%", SRV);
    
    if (_connections.size() > 0) {
        console::log("Connection FDs: ", SRV);
        for (const auto& pair : _connections) {
            console::log("  FD " + std::to_string(pair.first), SRV);
        }
    }
}
