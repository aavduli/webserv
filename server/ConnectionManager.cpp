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
		console::log("Attempted to remove non existent connection: " + std::to_string(clientFD));
		return ;
	 }
	 console::log("Removed connection: " + std::to_string(clientFD), SRV);
	 _eventManager.delFd(clientFD);
}
bool hasConnection(int clientFD) const{}
size_t getConnectionCount(){}

Conn& getConnection(int clientFD) {}
const Conn& getConnection(const int clientFD) {}
void removeAllConnection() {}
std::vector<int> getConnectionFds() const {}

bool isConnectionValid() const {}
void cleanUpStaleConn() {}

void printConnectionStats() const {}
