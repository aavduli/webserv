#ifndef SERVER_HPP
# define SERVER_HPP
# include <iostream>
# include <string>
# include <fstream>
# include <sys/socket.h>
# include <cstdlib>
# include <netinet/in.h>
# include "../console/console.hpp"
# include "eventManager.hpp"
# include "onConnection.hpp"
# include <cstring>
# include <fcntl.h>
# include <csignal>
# include <vector>

class server {
	private:
		struct sockaddr_in	_address;
		int					_port;
		int 				_serverfd;
		eventManager		_ev;
		std::map<int, Conn> _conns;

	private:
		void setServer();
		void setSockaddr();
		static int make_nonblock(int fd);
		static void ignore_sigpipe();
	public:
		server(int port);
		~server();

		void serverManager(WebservConfig &config);

	public:
	int getPort();
	int getServerFd();
};

#endif
