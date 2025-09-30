#include "NetworkHandler.hpp"

void NetworkHandler::logNetworkError(const std::string &operation, const std::string &error) {
	console::log(operation + error, ERROR);
}

//do operation on FD. SET_FL == file status FLAG / F_SETFD == file descriptor flags FD_CLOEXEC = atomicaly close the fd on exec
int NetworkHandler::makeNonblocking(int fd) {
	int fl = fcntl(fd, F_GETFL, 0);
	if (fl == -1) return -1;
	if (fcntl (fd, F_SETFL, fl | O_NONBLOCK) == -1) return -1;

	int clo = fcntl(fd, F_GETFD, 0);
	if (clo == -1) return -1;
	if (fcntl(fd, F_SETFD, clo | FD_CLOEXEC) == -1) return -1;
	
	return 0;
}

int NetworkHandler::createServersocket() {
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
	logNetworkError("[NETWORK][SOCKET] ", std::strerror(errno));
	return fd;
}

//make socket reusable
void NetworkHandler::setuptSocketOptions(int fd) {
	int yes = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
	#ifdef SO_REUSEPORT
	setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(yes));
	#endif
	if (makeNonblocking(fd) == -1)
		logNetworkError("[NETWORK][SOCKET]", std::strerror(errno));
}

//bind socket and listen what is comming
void NetworkHandler::bindAndListen(int serverfd, const struct sockadd_in& address) {
	if (bind(serverfd, (struct sockaddr*)&address, sizeof(&address)) < 0)
		logNetworkError("[NETWORK][BIND]", std::strerror(errno));
	if (listen(serverfd, SOMAXCONN) < 0)
		logNetworkError("[NETWORK][LISTEN]", std::strerror(errno));
}

//when connection, accept it and make it non block
int NetworkHandler::acceptConnection(int fd, struct sockaddr_storage &clientAddr) {
	socklen_t cientLenght = sizeof(clientAddr);
	int clientFD = accept(fd, (struct sockaddr*)&clientAddr, &cientLenght);
	if (clientFD == -1)
		logNetworkError("[NETWORK][ACCEPT]", std::strerror(errno));
	makeNonblocking(clientFD);
	return clientFD;
}
 
void NetworkHandler::closeConnection(int fd) {
	if (fd > 0) {
		close(fd);
	}
}

ssize_t NetworkHandler::receiveData(int fd, char *buffer, ssize_t size) {
	ssize_t recBytes = recv(fd, buffer, sizeof(buffer), size);
	return recBytes;
}

ssize_t NetworkHandler::sendData(int fd, char *buffer, ssize_t size) {
	ssize_t sendBytes = send(fd, buffer, sizeof(buffer), size);
	return sendBytes;
}

ssize_t NetworkHandler::sendFullData(int fd, char *buffer, ssize_t size) {
	ssize_t sendBytes = send(fd, buffer + size, sizeof(buffer) - size, 0);
	return sendBytes;
}

void NetworkHandler::ignoreSigPipe() {
	struct sigaction sa;
	std::memset(&sa, 0, sizeof(sa));
	sa.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &sa, NULL);
}

bool NetworkHandler::isSocketError(int fd) {
	if (fd == -1) return true;
	return false;
}
