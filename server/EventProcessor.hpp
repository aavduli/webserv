#pragma once

//Handle Epoll events

#include "../console/console.hpp"
#include <iostream>
#include "eventManager.hpp"
#include "NetworkHandler.hpp"
#include "ConnectionManager.hpp"
#include "server.hpp"

class eventProcessor {
	private:
		eventManager& _eventManager;
		connectionManager& _connectionManager;
		RequestProcessor& _requestProcessor; //todo
		int _serverFd;
		bool _shouldStop;

	private:
		void acceptNewConnections();
		void handleReceiveError(int clientFd, ssize_t recvResult);
		void sendResponse(int clientFd, const std::string response);

	public:
		eventProcessor(eventManager& em, connectionManager& cm, int serverFd);
		~eventProcessor();

		void runEventLoop(const WebservConfig& config);
		void stopEventLoop();

		void handleServerEvents(int serverFd);
		void handleClientDisconnection(int clientFd);
		void handleClientData(int clientFd, const WebservConfig& config);
		void handleClientWriteReady(int clientFd);

		bool isServerSocket(int fd) const;
		bool isDisconnectionEvent(uint32_t event) const;
		bool isDataReadyEvent(uint32_t event) const;
		bool isDataSendEvent(uint32_t event) const;
};
