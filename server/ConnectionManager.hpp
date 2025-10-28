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
#include "ServerConstants.hpp"
#include "NetworkHandler.hpp"


std::string intToString(int value);

class connectionManager {
	private:
		std::map<int, Conn> _connections;
		size_t _maxConn;
		eventManager& _eventManager;
	public:
		connectionManager(eventManager& em, size_t maxConn);
		~connectionManager();

		bool addConnection(int clientFD, int ServerFD);
		void removeConnection(int clientFD);
		bool hasConnection(int clientFD) const;
		size_t getConnectionCount();
		std::vector<int> getConnectionsForServer(int serverFD) const;
		int getConnectionCountForServer(int serverFD) const;

		Conn& getConnection(int clientFD);
		void updateActivity(int clientFD);
		std::vector<int> getTimedOutConnection(time_t currentTime);
		const Conn& getConnection(int clientFD) const;
		void removeAllConnection();
		std::vector<int> getConnectionFds() const;

		bool isConnectionValid(int clientFD) const;
		void cleanUpStaleConn();

		void printConnectionStats() const;
};
