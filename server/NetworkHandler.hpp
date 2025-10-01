#pragma once

//Handle pure network I/O operations

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
		NetworkHandler() = delete; //static class only
		static void logNetworkError(const std::string& operation, const std::string& error);
	public:
		//server creation and configuration
		static int createServerSocket();
		static void setupSocketOptions(int fd);
		static int makeNonblocking(int fd);
		static int bindAndListen(int serverFd, const struct sockaddr_in& address);

		//accept connection
		static int acceptConnection(int serverFd, struct sockaddr_storage& clientAddr); 
		static void closeConnection(int fd);

		//handle data
		static ssize_t receiveData(int fd, char *buffer, ssize_t size);
		static ssize_t sendData(int fd, char *buffer, ssize_t size);
		static ssize_t sendFullData(int fd, char *buffer, ssize_t size);
		
		//utility
		static void ignoreSigPipe();
		static bool isSocketError(int fd); 

		//Address
		static struct sockaddr_in createSockkaddr(int port);
};
