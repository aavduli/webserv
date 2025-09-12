#ifndef SERVER_HPP
# define SERVER_HPP
# include <iostream>
# include <string>
# include <fstream>
# include <sys/socket.h>
# include <cstdlib>
# include <netinet/in.h>
# include "../console/console.hpp"
# include "../event/eventManager.hpp"
# include <cstring>
# include <fcntl.h>
# include <csignal>
# include <vector>

#include "../messages/data/HttpMessage.hpp"
#include "../messages/MessageStreams.hpp"

class server {
	private:
		struct sockaddr_in	_address;
		int					_port;
		int 				_serverfd;
		eventManager		_ev;
	private:
		void setServer();
		void setSockaddr();
		static int make_nonblock(int fd);
		static void ignore_sigpipe();
	public:
		server(int port);
		~server();

		void serverManager();
};

#endif
