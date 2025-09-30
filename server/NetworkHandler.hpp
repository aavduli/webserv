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

class NetworkHandler {
	private:
		NetworkHandler() = delete; //static class only
		static void logNetworkError(const std::string& operation, const std::string& error); //todo
	public:
		//server creation and configuration
		static int createServersocket();
		static void setuptSocketOptions(int fd);
		static int makeNonblocking(int fd);
		static void bindAndListen(int serverFd, const struct sockadd_in& address);

		//accept connection
		static int acceptConnection(int serverFd, struct sockaddr_storage& clientAddr); 
		static void closeConnection(int fd);

		//handle data
		static ssize_t receiveData(int fd, char *buffer, ssize_t size);
		static ssize_t sendData(int fd, char *buffer, ssize_t size);
		static ssize_t sendFullData(int fd, char *buffer, ssize_t size);
		
		//utility
		static void ignoreSigPipe(); //todo
		static bool isSocketError(int fd); //todo

		//Address
		static struct sockkaddr_in createSockkaddr(int port); //todo
};
