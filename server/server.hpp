#ifndef SERVER_HPP
# define SERVER_HPP
# include <sys/socket.h>
# include <netinet/in.h>

#include "../messages/MessageStreams.hpp"

class server {
protected:
	int _server_fd;
	int _newsocket;
	int _port;
	struct sockaddr_in _addr;
public:
	server(int port);
	~server();

	void serverManager();
};

#endif
