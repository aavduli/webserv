#include "server.hpp"

int server::_shutdown_requested = 0;
struct sigaction server::_sa;

server::server(const std::vector<int>& port) : _port(port) {}

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
	std::memset(&_sa, 0, sizeof(_sa));
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

	if (!NetworkHandler::initializeServer(_port)) {
		console::log("Failed to initialize servers on any port", ERROR);
		return ;
	}
	const std::vector<int>& serverSocket = NetworkHandler::getServerSocket();
	std::cout << GREEN << "server listening on port:" << std::endl;
	for (size_t i = 0; i < _port.size(); ++i) {
		std::cout << _port[i] << " ";
	}
	std::cout << RESET << std::endl;

	eventManager evMng(1024);
	connectionManager cmMng(evMng, 1000);
	for (size_t i = 0; i < serverSocket.size(); i++) {
		int serverFd = serverSocket[i];
		evMng.addFd(serverFd, EPOLLIN);
	}
	eventProcessor evPro(evMng, cmMng, NetworkHandler::getServerSocket());
	console::log("Starting Event loop", SRV);
	evPro.runEventLoop(config);
	console::log("Event Loop stopped .... starting disconnection", SRV);
	NetworkHandler::cleanup();
	if (_shutdown_requested) {
		std::cout << YELLOW << "\nShutdown requested... bye !" << RESET << std::endl;
	}
}

const std::vector<int>& server::getPort() const {
	return _port;
}

int server::getShutDownRequest() {
	return _shutdown_requested;
}

void server::setShutDownRequest() {
	_shutdown_requested = 1;
}
