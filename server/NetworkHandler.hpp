#pragma once

//Handle pure network I/O operations

#include "server.hpp"
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <csignal>

class NetworkHandler : public server {
	private:
		NetworkHandler() = delete; //static class only
		static void logNetworkError(const std::string& operation, const std::string& error); //todo
	public:
		//server creation and configuration

		static int createServersocket();
		static void setuptSocketOptions(int fd);
		static int makeNonblocking(int fd);
		static void bindAndListen(int serverFd, const struct sockadd_in& address); //todo

		//accept connection
		static int acceptConnection(int serverFd, struct sockaddr_storage& clientAddr); //todo
		static int closeConnection(int fd); //todo

		//handle data
		static ssize_t receiveData(int fd, char *buffer, ssize_t size); //todo
		static ssize_t sendData(int fd, char *buffer, ssize_t size); //todo
		static ssize_t sendFullData(int fd, char *buffer, ssize_t size); //todo
		
		//utility
		static void ignoreSigPipe(); //todo
		static bool isSocketError(int fd); //todo

		//Address
		static struct sockkaddr_in createSockkaddr(int port); //todo
};
