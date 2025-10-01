#pragma once

//handle connection lifecycle
#include <iostream>
#include <sstream>
#include "../console/console.hpp"
#include "onConnection.hpp"
#include <map>
#include <vector>
#include <stdexcept>
#include "eventManager.hpp"
#include "NetworkHandler.hpp"


class connectionManager {
	private:
		std::map<int, Conn> _connections;
		size_t _maxConn;
		eventManager& _eventManager;
	public:
		connectionManager(eventManager& em, size_t maxConn);
		~connectionManager();

		bool addConnection(int clientFD);
		void removeConnections(int clientFD);
		bool hasConnection(int clientFD) const;
		size_t getConnectionCount();

		Conn& getConnection(int clientFD);
		const Conn& getConnection(const int clientFD);
		void removeAllConnection();
		std::vector<int> getConnectionFds() const;

		bool isConnectionValid(int clientFD) const;
		void cleanUpStaleConn();

		void printConnectionStats() const;
};
