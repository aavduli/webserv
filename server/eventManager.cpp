#include "eventManager.hpp"

eventManager::eventManager(int max_events) :  _epfd(-1), _events(NULL), _maxEvents(max_events > 0 ? max_events : 64) {
	_epfd = epoll_create1(0);
	if (_epfd == -1) {
		std::cerr << RED << "epoll_create1 failed" << RESET << std::endl;
		_maxEvents = 0;
		return ;
	}
	_events = new epoll_event[_maxEvents];
	std::memset(_events, 0, sizeof(epoll_event) * _maxEvents);
}

eventManager::~eventManager() {
	if (_epfd != -1)
		close(_epfd);
	delete[] _events;
}

int eventManager::getFd() const {
	return _epfd;
}

int eventManager::wait(int timeout) {
	if (_epfd == -1) { errno = EBADF; return -1; }
	return epoll_wait(_epfd, _events, _maxEvents, timeout);
}

epoll_event& eventManager::operator[](int i) {
	return _events[i];
}

void eventManager::addFd(int fd, uint32_t events) {
	struct epoll_event ev;
	std::memset(&ev, 0, sizeof(ev));
	ev.events = events;
	ev.data.fd = fd;
	if (epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &ev) == -1)
		std::cerr << RED << "epoll_ctl ADD failed" << RESET << std::endl;
}

void eventManager::modFd(int fd, uint32_t events) {
	struct epoll_event ev;
	std::memset(&ev, 0, sizeof(ev));
	ev.events = events;
	ev.data.fd = fd;
	if (epoll_ctl(_epfd, EPOLL_CTL_MOD, fd, &ev) == -1)
		std::cerr << RED << "epoll_ctl_mod failed" << RESET << std::endl;
}

void eventManager::delFd(int fd) {
	if (epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, NULL) == -1)
		std::cerr << RED << "epoll_ctl_del failed" << RESET << std::endl;
}
