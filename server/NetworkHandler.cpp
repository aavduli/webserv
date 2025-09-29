#include "NetworkHandler.hpp"

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

int NetworkHandler::setuptSocketOptions(int fd) {
	int yes = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
	#ifdef SO_REUSEPORT
	setsocketopt(fd, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(yes));
	#endif
	if (makeNonblocking(fd) == -1)
		logNetworkError("[NETWORK][SOCKET]", std::string(errno));
}

int NetworkHandler::createServersocket() {
	getServerFd() = socket(AF_INET, SOCK_STREAM, 0);
	if (getServerFd() < 0)
		logNetworkError("[NETWORK][SOCKET] ", std::string(errno));
	setuptSocketOptions(getServerFd());
}

