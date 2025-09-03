#include "server.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include "../console/console.hpp"
#include <cstdlib>
#include <cstring>
#include <unistd.h>

server::server(int port) : _port(port) {}

server::~server() {}

void server::serverManager() {
	_server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_server_fd == -1) {
		console::log("failed to create socket", ERROR);
		std::cout << errno << std::endl;
		exit(EXIT_FAILURE);
	}
	memset(&_addr, 0, sizeof(_addr));
	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = INADDR_ANY;
	_addr.sin_port = htons(_port);

	if (bind(_server_fd, (struct sockaddr*)&_addr, sizeof(_addr)) < 0) {
		console::log("Failed to bind port", ERROR);
		std::cout << errno << std::endl;
		exit(EXIT_FAILURE);
	}

	if (listen(_server_fd, 1024) < 0) {
		console::log("Failed to listen on the socket: ", ERROR);
		std::cout << errno << std::endl;
		exit(EXIT_FAILURE);
	}
	console::log("Server listening", INFO);
	size_t addrlen = sizeof(_addr);
	int connection = accept(_server_fd, (struct sockaddr*)&_addr, (socklen_t*)&addrlen);
	if (connection < 0) {
		console::log("Failed to grab connection", ERROR);
		std::cout << errno << std::endl;
		exit(EXIT_FAILURE);
	}
	char buffer[100];
	read(connection, buffer, 100);
	console::log(buffer, INFO);

	std::string response = "Connection 5/5\n";
	send(connection, response.c_str(), response.size(), 0);
	close(connection);
	close(_server_fd);
}
