#pragma once

//Handle Epoll events

#include "../console/console.hpp"
#include <iostream>
#include "eventManager.hpp"
#include "ConnectionManager.hpp"
#include "server.hpp"

class eventProcessor {
	private:
		eventManager& _eventManager;
		connectionManager& _connectionManager;
		RequestProcessor& _requestProcessor; //todo
		int _serverFd;

	private:
		void acceptNewConnections();
		void processIncomingData(int clientFd, const WebservConfig& config);
		void handleReceiveError(int clientFd, ssize_t recvResult);
		void sendResponse(int clientFd, const std::string response);

	public:
	eventProcessor(eventManager& em, connectionManager& cm, int serverFd);
	~eventProcessor();

	void runEventLoop(const Webservconfig& config);
	void stopEventLoop();

	void handleServerEvents(int serverFd);
	void handleClientDisconnection(int clientFd); //EPOLLUP | EPOLLERR | EPOLLRDUP
	void handleServerDisconnection(int serverFd);

	

};
