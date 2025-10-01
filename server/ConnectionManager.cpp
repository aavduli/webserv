#include "ConnectionManager.hpp"

connectionManager::connectionManager(eventManager& em, size_t maxConn) :_maxConn(maxConn), _eventManager(em) {
	if (maxConn < 0)
		throw std::runtime_error("Max connections has to be at least 1");
	console::log("ConnectionManager initialized with max connections: " + intToString(maxConn), SRV);
}

connectionManager::~connectionManager() {}

std::string intToString(int value) {
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

bool connectionManager::addConnection(int clientFD) {
	if (_connections.size() >=  _maxConn) {
		console::log("Max connection has been reached", SRV);
		return false;
	}
	if (_connections.find(clientFD) != _connections.end()) {
		console::log("Connection already exists for FD: " + intToString(clientFD), ERROR);
		return false;
	}
	_connections[clientFD] = Conn();
	console::log("Add connection for FD: " + intToString(clientFD), SRV);
	return true;
}

void connectionManager::removeConnections(int clientFD) {
	 std::map<int, Conn>::iterator it = _connections.find(clientFD);
	 if (it == _connections.end()) {
		console::log("Attempted to remove non existent connection: " + intToString(clientFD), ERROR);
		return ;
	 }
	 console::log("Removed connection: " + intToString(clientFD), SRV);
	 _eventManager.delFd(clientFD);
	 NetworkHandler::closeConnection(clientFD);
	 _connections.erase(it);
}

bool connectionManager::hasConnection(int clientFD) const {
	return _connections.find(clientFD) != _connections.end();
}

size_t connectionManager::getConnectionCount() {
	return _connections.size();
}

Conn& connectionManager::getConnection(int clientFD) {
	std::map<int, Conn>::iterator it = _connections.find(clientFD);
	if (it == _connections.end()) {
		throw std::runtime_error("Connection not found for fd: " + intToString(clientFD));
	}
	return it->second;
}

const Conn& connectionManager::getConnection(const int clientFD) {
	std::map<int, Conn>::const_iterator it = _connections.find(clientFD);
	if (it == _connections.end()) {
		throw std::runtime_error("Connection not found for fd: " + intToString(clientFD));
	}
	return it->second;
}

void connectionManager::removeAllConnection() {
	console::log("Removing all connection: " + intToString(_connections.size()), SRV);
	for (std::map<int, Conn>::iterator it = _connections.begin(); it != _connections.end(); ++it) {
		int fd = it->first;
		_eventManager.delFd(fd);
		NetworkHandler::closeConnection(fd);
	}
	_connections.clear();
}

std::vector<int> connectionManager::getConnectionFds() const {
	std::vector<int> fds;
	fds.reserve(_connections.size());
	for(std::map<int, Conn>::const_iterator it = _connections.begin(); it != _connections.end(); ++it) {
		fds.push_back(it->first);
	}
	return fds;
}

bool connectionManager::isConnectionValid(int clientFD) const {
	if (!hasConnection(clientFD)) return false;
	return NetworkHandler::isSocketError(clientFD) == false;
}

void connectionManager::cleanUpStaleConn() {
	std::vector<int> staleConn;
	for (std::map<int, Conn>::iterator it = _connections.begin(); it != _connections.end(); ++it) {
		int fd = it->first;
		if (NetworkHandler::isSocketError(fd))
			staleConn.push_back(fd);
	}
	for (std::vector<int>::iterator it = staleConn.begin(); it != staleConn.end(); ++it) {
		int fd = *it;
		console::log("Removing stale connection: " + intToString(fd), SRV);
		removeConnections(fd);
	}
}

void connectionManager::printConnectionStats() const {
    console::log("=== Connection Statistics ===", SRV);
    console::log("Active connections: " + intToString(_connections.size()), SRV);
    console::log("Max connections: " + intToString(_maxConn), SRV);
    console::log("Capacity used: " + intToString((_connections.size() * 100) / _maxConn) + "%", SRV);
    
    if (_connections.size() > 0) {
        console::log("Connection FDs: ", SRV);
        for (std::map<int, Conn>::const_iterator it = _connections.begin(); it != _connections.end(); ++it) {
            console::log("  FD " + intToString(it->first), SRV);
        }
    }
}
