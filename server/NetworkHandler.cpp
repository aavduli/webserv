#include "NetworkHandler.hpp"

void NetworkHandler::logNetworkError(const std::string &operation, const std::string &error) {
	console::log(operation + error, ERROR);
}

//do operation on FD. SET_FL == file status FLAG / F_SETFD == file descriptor flags FD_CLOEXEC = atomicaly close the fd on exec
int NetworkHandler::makeNonblocking(int fd) {
	int fl = fcntl(fd, F_GETFL, 0);
	if (fl == -1) return NON_BLOCK_ERROR;
	if (fcntl (fd, F_SETFL, fl | O_NONBLOCK) == -1) return NON_BLOCK_ERROR;

	int clo = fcntl(fd, F_GETFD, 0);
	if (clo == -1) return -1;
	if (fcntl(fd, F_SETFD, clo | FD_CLOEXEC) == -1) return NON_BLOCK_ERROR;

	return NET_SUCCESS;
}

int NetworkHandler::createServerSocket() {
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		logNetworkError("[NETWORK][SOCKET] ", std::strerror(errno));
		return SOCKET_ERROR;
	}
	if (makeNonblocking(fd) != NET_SUCCESS) {
		logNetworkError("[NETWORK][SOCKET]", std::strerror(errno));
		close(fd);
		return SOCKET_ERROR;
	}
	return fd;
}

//make socket reusable
void NetworkHandler::setupSocketOptions(int fd) {
	int yes = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
		logNetworkError("[NETWORK][SOCK_OPT]", "SO_REUSEADDR FAILED");
	#ifdef SO_REUSEPORT
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(yes)) < 0)
		logNetworkError("[NETWORK][SOCK_OPT]", "SO_REUSEPORT FAILED");
	#endif
}

//bind socket and listen what is comming
int NetworkHandler::bindAndListen(int serverfd, const struct sockaddr_in& address) {
	if (bind(serverfd, (struct sockaddr*)&address, sizeof(address)) < 0) {
		logNetworkError("[NETWORK][BIND]", std::strerror(errno));
		return BIND_ERROR;
	}
	if (listen(serverfd, SOMAXCONN) < 0) {
		logNetworkError("[NETWORK][LISTEN]", std::strerror(errno));
		return LISTEN_ERROR;
	}
	return NET_SUCCESS;
}

//when connection, accept it and make it non block
int NetworkHandler::acceptConnection(int fd, struct sockaddr_storage &clientAddr) {
	socklen_t clientLength = sizeof(clientAddr);
	int clientFD = accept(fd, (struct sockaddr*)&clientAddr, &clientLength);
	console::log("[CLIENTFD][ACCEPTED]", clientFD, SRV);
	if (clientFD == -1) {
		logNetworkError("[NETWORK][CLIENTFD]", std::strerror(errno));
		return ACCEPT_ERROR;
	}
	if (makeNonblocking(clientFD) != NET_SUCCESS) {
		close(clientFD);
		return NON_BLOCK_ERROR;
	}
	return clientFD;
}

void NetworkHandler::closeConnection(int fd) {
	if (fd > 0) {
		console::log("[CLIENTFD][CLOSED]", fd, SRV);
		close(fd);
	}
}

ssize_t NetworkHandler::receiveData(int fd, char *buffer, ssize_t size) {
	ssize_t recBytes = recv(fd, buffer, size, 0);
	return recBytes;
}

ssize_t NetworkHandler::sendData(int fd, char *buffer, ssize_t size) {
	ssize_t sendBytes = send(fd, buffer, size, 0);
	return sendBytes;
}

//For handling parsed data send
ssize_t NetworkHandler::sendFullData(int fd, char *buffer, ssize_t remainingBytes) {
	ssize_t totalSend = 0;
	while (totalSend < remainingBytes) {
		ssize_t sent = send(fd, buffer + totalSend, remainingBytes - totalSend, 0);
		if (sent < 0) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) continue;
			return sent;
		}
		if (sent == 0) break;
		totalSend += sent;
	}
	return totalSend;
}

void NetworkHandler::ignoreSigPipe() {
	struct sigaction sa;
	std::memset(&sa, 0, sizeof(sa));
	sa.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &sa, NULL);
}

bool NetworkHandler::isSocketError(int fd) {
	if (fd == -1) return true;
	
	// Check if socket is actually valid by trying to get socket options
	int error = 0;
	socklen_t len = sizeof(error);
	int result = getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &len);
	
	if (result != 0) return true;  // getsockopt failed
	if (error != 0) return true;   // Socket has an error condition
	
	return false;
}

struct sockaddr_in NetworkHandler::createSockkaddr(int port) {
	struct sockaddr_in address;
	std::memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(port);
	return address;
}
