#pragma once

#include <netinet/in.h>
#include <sys/socket.h>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include "../console/console.hpp"

enum NetworkError {
	NET_SUCCESS = 0,
	SOCKET_ERROR = -1,
	BIND_ERROR = -2,
	LISTEN_ERROR = -3,
	ACCEPT_ERROR = -4,
	NON_BLOCK_ERROR = -5
};

class NetworkHandler {
	private:
		static std::vector<int> _server_socket;
		NetworkHandler();
		static void logNetworkError(const std::string& operation, const std::string& error);
	public:
		static bool initializeServer(std::vector<int>& ports);
		static int createServerSocket();
		static void setupSocketOptions(int fd);
		static int makeNonblocking(int fd);
		static int bindAndListen(int serverFd, const struct sockaddr_in& address);

		static int acceptConnection(int serverFd, struct sockaddr_storage& clientAddr); 
		static void closeConnection(int fd);

		static ssize_t receiveData(int fd, char *buffer, ssize_t size);
		static ssize_t sendData(int fd, char *buffer, ssize_t size);
		
		static void ignoreSigPipe();
		static bool isSocketError(int fd); 

		static struct sockaddr_in createSockkaddr(int port);
		static void cleanup();

		static bool isServerFd(int fd);
		static const std::vector<int>& getServerSocket();
};
