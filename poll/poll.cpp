#include "poll.hpp"
#include "../console/console.hpp"

pollEvents::~pollEvents() {}


void pollEvents::managePoll() {
	server_poll.fd = _server_fd;
	server_poll.events = POLLIN;
	poll_fds.push_back(server_poll);

	while (true) {
		int ret = poll(&poll_fds[0], poll_fds.size(), -1);
		if (ret < 0) {
			console::log("poll failed", ERROR);
			break;
		}
		
	}
}
