#include "server.hpp"

server::server(int port) : _port(port) {}

server::~server() {}

void server::serverManager(WebservConfig& config) {
	console::log("===STARTING WEBSERVER===", SRV);
	NetworkHandler::ignoreSigPipe();

	int serverFd = NetworkHandler::createServerSocket();
	NetworkHandler::setupSocketOptions(serverFd);
	struct sockaddr_in address = NetworkHandler::createSockkaddr(_port);
	if (NetworkHandler::bindAndListen(serverFd, address) != NET_SUCCESS) {
		console::log("Failed to bind and listen on port", ERROR);
		NetworkHandler::closeConnection(serverFd);
		return ;
	}
	std::cout << GREEN << "server listening on port:" << _port << RESET << std::endl;
	eventManager evMng(1024);
	connectionManager cmMng(evMng, 1000);
	eventProcessor evPro(evMng, cmMng, serverFd);
	evMng.addFd(serverFd, EPOLLIN);
	console::log("Starting Event loop", SRV);
	evPro.runEventLoop(config);
	console::log("Event Loop stopped .... starting disconnection", SRV);
}

int server::getPort() const {
	return _port;
}
