#pragma once

//Handle Epoll events

#include "../console/console.hpp"
#include <iostream>
#include "eventManager.hpp"
#include "../config/WebservConfig.hpp"
#include "NetworkHandler.hpp"
#include "ConnectionManager.hpp"
#include "server.hpp"
#include "ServerConstants.hpp"
#include "../messages/handling/MessageHandler.hpp"

class eventProcessor {
	private:
		eventManager& _eventManager;
		connectionManager& _connectionManager;
		int _serverFd;
		bool _shouldStop;

	private:
		void acceptNewConnections();
		void handleReceiveError(int clientFd, ssize_t recvResult);
		void sendResponse(int clientFd, const std::string& response);

	public:
		eventProcessor(eventManager& em, connectionManager& cm, int serverFd);
		~eventProcessor();

		void runEventLoop(const WebservConfig& config);
		void stopEventLoop();

		void handleServerEvents();
		void handleClientDisconnection(int clientFd);
		void handleClientData(int clientFd, const WebservConfig& config);
		void handleClientWriteReady(int clientFd);

		bool isServerSocket(int fd) const;
		bool isDisconnectionEvent(uint32_t event) const;
		bool isDataReadyEvent(uint32_t event) const;
		bool isDataSendEvent(uint32_t event) const;
};
