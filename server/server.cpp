#include "server.hpp"

static void give_to_angela(const std::string &raw) {
	std::cout << GREEN << "===BEGIN RAW TEST====" << std::endl;
	std::cout << raw;
	if (!raw.empty() && raw[raw.size() - 1] != '\n')
		std::cout << std::endl;
	std::cout << "====END OF RAW====" << RESET << std::endl;
}

server::server(int port) : _port(port), _serverfd(-1), _ev(1024)  {}

server::~server() {
	if (_serverfd != -1)
		close(_serverfd);
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

	if (bind(_serverfd, (struct sockaddr*)&_address, sizeof(_address)) < 0) {
		console::log("failed to bind", ERROR);
		exit(1);
	}
	if (listen(_serverfd, SOMAXCONN) < 0) {
		console::log("failed to listen", ERROR);
		exit(1);
	}
	std::cout << GREEN << "server listening on: " << _port << RESET << std::endl;

	_ev.addFd(_serverfd, EPOLLIN);
	while (true) {
		int nfds = _ev.wait(-1);
		if (nfds < 0) {
			if (errno == EINTR) continue;
			console::log("epoll_wait: ", ERROR); std::cout << std::strerror(errno) << std::endl;
			break ;
		}
		for (int i = 0; i < nfds; ++i) {
			int fd = _ev[i].data.fd;
			uint32_t events = _ev[i].events;
			if (fd == _serverfd) {
				while (true) {
					struct sockaddr_storage ca;
					socklen_t cl =sizeof(ca);
					int cfd = accept(_serverfd, (struct sockaddr*)&ca, &cl);
					if (cfd == -1) {
						if (errno == EAGAIN || errno == EWOULDBLOCK) break;
						console::log("ACCEPT: ", ERROR); std::cout << std::strerror(errno) << std::endl;
						break;
					}
					if (make_nonblock(cfd) == -1) {
						console::log("non block(client)", ERROR); std::cout << std::strerror(errno) << std::endl;
						close(cfd);
						continue;
					}
					_ev.addFd(cfd, EPOLLIN | EPOLLRDHUP);
					_conns[cfd] = Conn();
				}
				continue;
			}
			if (events & (EPOLLHUP | EPOLLERR | EPOLLRDHUP)) {
				std::cout << "[DEBUG] EPOLLHUP/ERR/RDHUP for fd " << fd << std::endl;
				_ev.delFd(fd);
				std::cout << "[DEBUG] Closing fd " << fd << std::endl;
				close(fd);
				std::cout << "[DEBUG] Erasing connection for fd " << fd << std::endl;
				_conns.erase(fd);
				continue;
			}
			if (events & EPOLLIN) {
				std::cout << "[DEBUG] EPOLLIN for fd " << fd << std::endl;
				Conn &c = _conns[fd]; // safe: creates if not present
				char buff[8192];
				bool alive = true;

				while (true) {
					ssize_t n = recv(fd, buff, sizeof(buff), 0);
					if (n > 0) {
						size_t sent = 0;
						while (sent < sizeof(buff) -1) {
							ssize_t s = send(fd, buff + sent, (sizeof(buff) - 1) - sent, 0
#ifdef MSG_NOSIGNAL
							| MSG_NOSIGNAL
#endif
							);
							if (s > 0) sent += (size_t)s;
							else if (s == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) break;
							else {close_it = true; break; }
						}
						close_it = true;
						break;
						}
					else if (n == 0) {
						close_it = true;
						c.in.append(buff, static_cast<size_t>(n));
						continue;
					}
					if (n == 0) {
						std::cout << "[DEBUG] recv returned 0 for fd " << fd << std::endl;
						size_t endpos;
						while (onConn::update_and_ready(c, endpos)) {
							give_to_angela(c.in.substr(0, endpos));
							c.in.erase(0, endpos);
							c.header_done = false;
							c.chunked = false;
							c.content_len = -1;
							c.body_have = 0;
							c.headers_end = std::string::npos;
						}
						_ev.delFd(fd);
						std::cout << "[DEBUG] Closing fd " << fd << std::endl;
						close(fd);
						std::cout << "[DEBUG] Erasing connection for fd " << fd << std::endl;
						_conns.erase(fd);
						alive = false;
						break;
					}
					if (n == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) break;
					if (n == -1 && errno == EINTR) continue;
					std::cout << "[DEBUG] recv error for fd " << fd << ", errno: " << errno << std::endl;
					_ev.delFd(fd);
					std::cout << "[DEBUG] Closing fd " << fd << std::endl;
					close(fd);
					std::cout << "[DEBUG] Erasing connection for fd " << fd << std::endl;
					_conns.erase(fd);
					alive = false;
					break;
				}
				if (!alive) continue;
				size_t endpos;
				while (onConn::update_and_ready(c, endpos)) {
					give_to_angela(c.in.substr(0, endpos));
					c.in.erase(0, endpos);
					c.header_done = false;
					c.chunked = false;
					c.content_len = -1;
					c.body_have = 0;
					c.headers_end = std::string::npos;
				}
				if (!c.header_done && c.in.size() > onConn::MAX_HEADER_BYTES
					&& onConn::headers_end_pos(c.in) == std::string::npos) {
					std::cout << "[DEBUG] Header too large for fd " << fd << std::endl;
					_ev.delFd(fd);
					std::cout << "[DEBUG] Closing fd " << fd << std::endl;
					close(fd);
					std::cout << "[DEBUG] Erasing connection for fd " << fd << std::endl;
					_conns.erase(fd);
					continue;
				}
			}
		}
	}
}
