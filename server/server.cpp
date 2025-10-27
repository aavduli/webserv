#include "server.hpp"

int server::_shutdown_requested = 0;
struct sigaction server::_sa;

server::server(int port) : _port(port) {}

server::~server() {}

void server::signalHandler(int signal) {
	if (signal == SIGTERM) {
		console::log("sigaction SIGTERM, shutting down....", SRV);
		server::setShutDownRequest();
	}
	else if (signal == SIGINT) {
		console::log("sigaction SIGINT, shutting down...", SRV);
		server::setShutDownRequest();
	}
}

void server::setupSignalHandler() {
	memset(&_sa, 0, sizeof(_sa));
	_sa.sa_handler = signalHandler;
	sigemptyset(&_sa.sa_mask);
	_sa.sa_flags = SA_RESTART;

	if (sigaction(SIGINT, &_sa, NULL) == -1) {
		console::log("sigaction SIGINT", ERROR);
		exit(1);
	}
	if (sigaction(SIGTERM, &_sa, NULL) == -1) {
		console::log("sigaction SIGTERM", ERROR);
		exit(1);
	}
}

void server::serverManager(WebservConfig& config) {
	console::log("===STARTING WEBSERVER===", SRV);
	server::setupSignalHandler();
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
	if (_shutdown_requested) {
		std::cout << YELLOW << "\nShutdown requested... bye !" << RESET << std::endl;
	}
}

int server::getPort() const {
	return _port;
}

int server::getShutDownRequest() {
	return _shutdown_requested;
}

void server::setShutDownRequest() {
	_shutdown_requested = 1;
}
