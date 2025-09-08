#include "server.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include "../console/console.hpp"
#include <cstdlib>
#include <cstring>
#include <unistd.h>

#include "../messages/MessageStreams.hpp"
#include "../messages/HttpMessage.hpp"

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
	
	// Keep the server running in a loop
	while (true) {
		size_t addrlen = sizeof(_addr);
		int connection = accept(_server_fd, (struct sockaddr*)&_addr, (socklen_t*)&addrlen);
		if (connection < 0) {
			console::log("Failed to grab connection", ERROR);
			std::cout << errno << std::endl;
			continue; // Continue to next iteration instead of exiting
		}
		
		char buffer[10];
		memset(buffer, 0, sizeof(buffer));
		ssize_t bytes_read = read(connection, buffer, sizeof(buffer) - 1);
		console::log(buffer, INFO);
		
		s_msg_streams	streams(std::cout, std::cout, std::cout);
		std::string		http_request;

		while (bytes_read > 0) {
			streams.simple << buffer;
			bytes_read = read(connection, buffer, sizeof(buffer) - 1);
		}
		handle_request(streams);

		// TODO: handle request line by line

		// TODO: switch case between server and parser to monitor parsing steps
		// Parsing state flag + status code (initially set to -1?) continuously updated to break ASAP if needed

		// TODO: Parse incoming HTTP request from buffer
		// TODO: Create HTTP response with status code, headers and output body if needed

		// HttpRequest request(buffer, bytes_read);
		// HttpResponse response(request);

		// TODO: encode response to stream format

		// TMP: Send a string HTTP response
		std::string http_response = 
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: text/html\r\n"
			"Content-Length: 85\r\n"
			"Connection: close\r\n"
			"\r\n"
			"<html><body><h1>Hello World!</h1><p>Your webserver is working!</p></body></html>";
		send(connection, http_response.c_str(), http_response.size(), 0);
		close(connection);
	}
	close(_server_fd);
}
