#ifndef POLL_HPP
# define POLL_HPP
# include <iostream>
# include <string>
# include <fstream>
# include <poll.h>
# include "../server/server.hpp"

class pollEvents : public server {
private:
	pollEvents();
protected:
	std::vector<struct pollfd> poll_fds;
	struct pollfd server_poll;
public:
	~pollEvents();

	void managePoll();

};

#endif
