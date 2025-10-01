#ifndef EVENTMANAGER_HPP
# define EVENTMANAGER_HPP
# include <iostream>
# include <string>
# include <fstream>
# include <stdlib.h>
# include <sys/epoll.h>
# include <unistd.h>
# include <cstring>
# include "../console/console.hpp"
# include <cstdlib>

class eventManager {
private:
	int _epfd;
	struct epoll_event *_events;
	int _maxEvents;
public:
	eventManager(int max_events);
	~eventManager();

	int getFd() const;
	int wait(int timeout);
	struct epoll_event& operator[](int i);

	void addFd(int fd, uint32_t events);
	void modFd(int fd, uint32_t events);
	void delFd(int fd);
};

#endif
