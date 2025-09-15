#include "server.hpp"
#include "../messages/MessageStreams.hpp"

server::server(int port) : _port(port), _serverfd(-1), _ev(1024)  {}

server::~server() {
	if (_serverfd != -1) close(_serverfd);
}

int server::make_nonblock(int fd) {
	int fl = fcntl(fd, F_GETFL, 0);
	if (fl == -1) return -1;
	if (fcntl(fd, F_SETFL, fl | O_NONBLOCK) == -1) return -1;

	int clo = fcntl(fd, F_GETFD);
	if (clo == -1) return -1;
	if (fcntl(fd, F_SETFD, clo | FD_CLOEXEC) == -1) return -1;
	return 0;
}

void server::ignore_sigpipe() { 
	struct sigaction sa;
	std::memset(&sa, 0, sizeof(sa));
	sa.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &sa, NULL);
}

void server::setServer() {
	if (_port < 0 || _port >= 65535)
		std::cerr << RED << "invalid port: " << YELLOW << _port << RESET << std::endl;
	_serverfd = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverfd < 0)
		console::log("failed to create socket", ERROR);
	int yes = 1;
	setsockopt(_serverfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
	#ifdef SO_REUSEPORT
	setsockopt(_serverfd, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(yes));
	#endif
	if (make_nonblock(_serverfd) == -1) {
		std::cerr << RED << "nonblock(listen):" << RESET << std::strerror(errno) << std::endl;
		exit(1);
	}
}

void server::setSockaddr() {
	std::memset(&_address, 0, sizeof(_address));
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = htonl(INADDR_ANY);
	_address.sin_port = htons(_port);
}

void server::serverManager() {
	ignore_sigpipe();
	setServer();
	setSockaddr();

	if (bind(_serverfd, (struct sockaddr *)&_address, sizeof(_address)) < 0) {
		std::cerr << RED << "failed to bind: " << std::strerror(errno) << RESET << std::endl;
		exit(1);
	}
	if (listen(_serverfd, SOMAXCONN) < 0) {
		std::cerr << RED << "failed to listen: " << std::strerror(errno) << RESET << std::endl;
		exit(1);
	}
	std::cout << GREEN << "server listening on: " << _port << RESET << std::endl;

	_ev.addFd(_serverfd, EPOLLIN);

	while (true) {
		int nfds = _ev.wait(-1);
		if (nfds < 0) {
			if (errno == EINTR)  continue;
			std::cerr << RED << "epoll_wait: " << std::strerror(errno) << RESET << std::endl;
			break;
		}
		for (int i = 0; i < nfds; ++i) {
			int fd = _ev[i].data.fd;
			uint32_t events = _ev[i].events;
			if (fd == _serverfd) {
				while (true) {
					struct sockaddr_storage ca;
					socklen_t cl = sizeof(ca);
					int cfd = accept(_serverfd, (struct sockaddr*)&ca, &cl);
					if (cfd == -1) {
						if (errno == EAGAIN || errno == EWOULDBLOCK) break;
						std::cerr << RED << "accept: " << std::strerror(errno) << RESET << std::endl;
						break;
					}
					if (make_nonblock(cfd) == -1) {
						std::cerr << YELLOW << "non block(client)" << std::strerror(errno) << RESET << std::endl;
						close(cfd);
						continue;
					}
					_ev.addFd(cfd, EPOLLIN | EPOLLRDHUP);
				}
				continue;
			}
			if (events & (EPOLLHUP | EPOLLERR | EPOLLRDHUP)) {
				_ev.delFd(fd);
				close(fd);
				continue;
			}
			if (events & EPOLLIN) {
				bool close_it = false;
				char buff[8096];
				std::string tmp;
				while (true) {
					ssize_t n = recv(fd, buff, sizeof(buff), 0);
					tmp.append(buff, n);
					std::cout << tmp << std::endl;
					if (n > 0) {
						size_t sent = 0;
						while (sent < sizeof(buff) -1) {
							ssize_t s = send(fd, buff + sent, (sizeof(buff) - 1) - sent, MSG_NOSIGNAL);
							if (s > 0) sent += (size_t)s;
							else if (s == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) break;
							else {close_it = true; break; }
						}
						close_it = true;
						break;
						}
					else if (n == 0) {
						close_it = true;
						break;
					}
					else {
						if (errno == EAGAIN  || errno == EWOULDBLOCK) break;
						if (errno == EINTR) continue;
						close_it = true;
						break;
					}
				}
				if (close_it) {
					_ev.delFd(fd);
					close(fd);
				}
			}
		}
	}
}
