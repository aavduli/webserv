#include "server.hpp"

server::server(int port) : _port(port), _serverfd(-1), _ev(1024)  {}

std::string build_http_response(
    int status_code,
    const std::string& status_text,
    const std::string& body,
    const std::string& content_type = "text/plain"
) {
    std::ostringstream oss;
    oss << "HTTP/1.1 " << status_code << " " << status_text << "\r\n"
        << "Content-Type: " << content_type << "\r\n"
        << "Content-Length: " << body.size() << "\r\n"
        << "Connection: close\r\n"
        << "\r\n"
        << body;
    return oss.str();
}

server::~server() {}

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
		console::log("Make nonblock: ", std::strerror(errno), ERROR);
		exit(1);
	}
}

void server::setSockaddr() {
	std::memset(&_address, 0, sizeof(_address));
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = htonl(INADDR_ANY);
	_address.sin_port = htons(_port);
}

void server::serverManager(WebservConfig &config) {
	ignore_sigpipe();
	setServer();
	setSockaddr();
	(void)config;
	int msgSend = 0;
	
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
			console::log("epoll_wait: ", std::strerror(errno), ERROR);
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
						console::log("ACCEPT: ", std::strerror(errno), ERROR);
						break;
					}
					if (make_nonblock(cfd) == -1) {
						console::log("non block(client)", std::strerror(errno), ERROR);
						continue;
					}
					_ev.addFd(cfd, EPOLLIN | EPOLLRDHUP);
					_conns[cfd] = Conn();
				}
				continue;
			}
			if (events & (EPOLLHUP | EPOLLERR | EPOLLRDHUP)) {
				_ev.delFd(fd);
				close(fd);
				_conns.erase(fd);
				continue;
			}
			if (events & EPOLLIN) {
				Conn &c = _conns[fd]; // safe: creates if not present
				char buff[8192];
				bool alive = true;
				while (true) {
					ssize_t n = recv(fd, buff, sizeof(buff), 0);
					console::log("Something has been received", SRV);
					if (n > 0) {
						c.in.append(buff, static_cast<size_t>(n));
						size_t endpos = c.in.size();
						handle_request(config, c.in.substr(0, endpos));
						while (onConn::update_and_ready(c, endpos)) {
							std::string response = build_http_response(200, "ok, Hello World!", "text/plain");
							size_t sent = 0;
							while (sent < response.size()) {
								ssize_t s = send(fd, response.data() + sent, response.size() - sent, 0
#ifdef MSG_NOSIGNAL
				| MSG_NOSIGNAL
#endif
			);
								msgSend++;
								if (s > 0) sent += (size_t)s;
								else break;
							}
							console::log("msgSend:", msgSend, SRV);
							break;
						}
					}
					if (n == 0) {
						_ev.delFd(fd);
						close(fd);
						_conns.erase(fd);
						alive = false;
						break;	
					}
					if (n == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)){
						_ev.delFd(fd);
						close(fd);
						_conns.erase(fd);
						alive = false;
					}
					if (n == -1 && errno == EINTR) {
						_ev.delFd(fd);
						close(fd);
						_conns.erase(fd);
						alive = false;
						break;
					}
					if (!alive) continue;
					size_t endpos = 0;
					onConn::onDiscon(c, alive, endpos);
					if (!c.header_done && c.in.size() > onConn::MAX_HEADER_BYTES
					&& onConn::headers_end_pos(c.in) == std::string::npos) {
						_ev.delFd(fd);
						close(fd);
						_conns.erase(fd);
						continue;
					}
				}
			}
		}
	}
}

int server::getPort() {
	return _port;
}
