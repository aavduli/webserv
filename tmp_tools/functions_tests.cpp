/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   functions_tests.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: angela <angela@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 12:47:06 by ahanzi            #+#    #+#             */
/*   Updated: 2025/09/01 10:22:54 by angela           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fstream>
#include <sstream>
#include <iostream>
#include <errno.h>			// errno, number of last error
#include <string.h>			// strerror
#include <cstdio>			// perror
#include <stdlib.h>			// exit
#include <unistd.h>			// execve, pipe, dup, dup2, close, read, write, access, chdir
#include <sys/wait.h>		// waitpid
#include <signal.h>			// kill, signal
#include <sys/stat.h>		// stat
#include <fcntl.h>			// fcntl, open
#include <dirent.h>			// opendir, readdir, closedir
#include <sys/socket.h>		// socketpair, socket, accept, listen, send, recv, bind, connect, setsockopt, getsockname
#include <arpa/inet.h>		// htonl, htons, ...
#include <sys/select.h>		// select
#include <poll.h>			// poll
#include <sys/epoll.h>		// epoll
#include <sys/types.h>		// kqueue, kevent
#include <sys/time.h>		// kqueue, kevent
#include <netdb.h>			// getaddrinfo, freeaddrinfo, gai_strerror, getprotobyname, getprotobynumber, getprotent
#include <netinet/in.h>		// sockaddr_in struct

int	main(void) {

	// ----------- Minimum setup needed by server in order to listen, accept, and speak with a client

	int		server_fd;					// listen for new connections
	int		new_socket;					// specific client that connects
	struct 	sockaddr_in address;		// struct to hold IP address and port information for the server
	int		opt = 1;					// option value for setsockopt 1 = SO_REUSEADDR (re-bind to port)
	int		addrlen = sizeof(address);	// size of the address struct to use with accept() later
	char	buffer[4000] = {0};			// buffer to store incoming data

	// ----------- Open server to incoming network connections

	// create socket fd with AF_INET=IPv4, SOCK_STREAM=TCP connection, 0=default protocol
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == 0) {
		perror("Socket failed");
		exit(EXIT_FAILURE);
	}

	// identity of the server
	address.sin_family = AF_INET;			// set address family to IPv4
	address.sin_addr.s_addr = INADDR_ANY;	// accept connections from any IP address on this machine
	address.sin_port = htons(8080);			// bind to port 8080

	// bind socket (server_fd) to specified IP address and port, defining its network address
	if (bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
		perror("Bind failed");		// fails if port already in use or no permission
		exit(EXIT_FAILURE);
	}

	// put the socket in listening mode, allows a stream socket to accept incoming connections
	// 3 = backlog -> how many incoming connections can be queued while waiting for accept()
	listen(server_fd, 3);

	std::cout << "Listening on port 8080...\n" << std::endl;

	// ----------- Wait until a client connects to the server_fd

	// stay running and accept multiple connections
	while (true) {

		std::cout << "Waiting for a connection...\n" << std::endl;

		// creates a new socket for incoming connection, accepts a connection from a peer application
		new_socket = accept(server_fd, (struct sockaddr *) &address, (socklen_t*) &addrlen);
		if (new_socket < 0) {
			perror("Accept failed");
			continue;
		}

		memset(buffer, 0, sizeof(buffer));		// avoid buffer overflow
		read(new_socket, buffer, 4000);			// read data by the client into buffer

		std::cout << "Received request:\n" << buffer;

		// ----------- Check request kind (parsing request line)

		// extract request line
		std::string	request(buffer);
		size_t		line_end = request.find("\r\n");
		std::string	request_line = request.substr(0, line_end);

		// split request line by spaces
		size_t	first_sp = request_line.find(" ");
		size_t	second_sp = request_line.find(" ", first_sp + 1);

		// retrieve method, path, version
		std::string	method = request_line.substr(0, first_sp);
		std::string	path = request_line.substr(first_sp + 1, second_sp - first_sp - 1);
		std::string	version = request_line.substr(second_sp + 1);

		std::cout << "Method: " << method << std::endl;
		std::cout << "Path: " << path << std::endl;
		std::cout << "HTTP Version: " << version << std::endl;

		// ----------- Response according to method

		if (method == "GET" && path == "/") {

			// serve index.html
			std::ifstream	file("index.html");		// assume file is in the same directory
			if (!file.is_open()) {
				std::cerr << "Failed to open index.html\n";
				const char*	not_found = 
					"HTTP/1.1 404 Not Found\r\n"
					"Content-Type: text/plain\r\n"
					"Content-Length: 13\r\n"
					"\r\n"
					"404 Not Found";
				write(new_socket, not_found, strlen(not_found));
			} else {

				// read the file into a string
				std::stringstream	buffer_stream;
				buffer_stream << file.rdbuf();
				std::string	body = buffer_stream.str();
				file.close();

				// create full HTTP response with correct content-length
				std::string	response = 
					"HTTP/1.1 200 OK\r\n"
					"Content-Type: text/html\r\n"
					"Content-Length: " + std::to_string(body.length()) + "\r\n"
					"\r\n" + body;
				write(new_socket, response.c_str(), response.length());
			}
			close(new_socket);
			std::cout << "GET request" << std::endl;

		}
		else if (method == "POST") 
			std::cout << "POST request" << std::endl;
		else if (method == "PUT") 
			std::cout << "PUT request" << std::endl;
		else if (method == "DELETE") 
			std::cout << "DELETE request" << std::endl;
		else {
			std::cout << "Unsupported method: " << method << std::endl;
			const char *response_405 =
				"HTTP/1.1 405 Method Not Allowed\r\n"
				"Content-Type: text/plain\r\n"
				"Content-Length: 18\r\n"
				"\r\n"
				"Method Not Allowed";
			write(new_socket, response_405, strlen(response_405));
			close(new_socket);
		}
	}
	close(server_fd);
	return 0;
}

// creates an unnamed pair of connected sockets in the specified domain, 
// int socketpair(int domain, int type, int protocol, int sv[2]);

// convert values between host and network byte order
// htonl, htons, ntohl, ntohs

// select() allows a program to monitor multiple file descriptors, waiting until one or more of the file descriptors become "ready" for some class of I/O operation.
// int select(int nfds, fd_set *_Nullable restrict readfds, fd_set *_Nullable restrict writefds, fd_set *_Nullable restrict exceptfds, struct timeval *_Nullable restrict timeout);

// similar task to select(2): waits for one of a set of file descriptors to become ready to perform I/O.
// int poll(struct pollfd *fds, nfds_t nfds, int timeout);

// The epoll API performs a similar task to poll(2), monitoring multiple file descriptors to see if I/O is possible on any of them.

// creates a new epoll instance and returns a file descriptor referring to that instance.
// int epoll_create(int size);

// used to add, modify, or remove entries in the interest list of the epoll(7) instance.
// int epoll_ctl(int epfd, int op, int fd, struct epoll_event *_Nullable event);

// waits for events on the epoll(7) instance referred to by the file descriptor epfd.
// int epoll_wait(int epfd, struct epoll_event events[.maxevents], int maxevents, int timeout);

// system call notifying the user when an event happens or a condition holds, based on the results of small pieces of kernel code termed filters.
// int kqueue(void);

// used to register events with the queue, and return any pending events to the user.
// int kevent(int  kq, const struct kevent *changelist, int nchanges, struct kevent *eventlist, int nevents, const struct timespec *timeout);

// creates an endpoint/unbound socket in a communications domain
// int socket(int domain, int type, int protocol);

// accept a new connection on a socket
// int accept(int sockfd, struct sockaddr *_Nullable restrict addr, socklen_t *_Nullable restrict addrlen);

// listen for connections on a socket
// int listen(int sockfd, int backlog);

// send a message on a socket
// ssize_t send(int sockfd, const void buf[.size], size_t size, int flags);

// receive a message from a socket
// ssize_t recv(int sockfd, void buf[.size], size_t size, int flags);

// bind a name to a socket
// int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

// initiate a connection on a socket
// int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

// getaddrinfo, freeaddrinfo, gai_strerror - network address and service translation
// int getaddrinfo(const char *restrict node, const char *restrict service, const struct addrinfo *restrict hints, struct addrinfo **restrict res);
// void freeaddrinfo(struct addrinfo *res);
// const char *gai_strerror(int errcode);

// set the socket options
// int setsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_len);

// getprotobyname, getprotobynumber, getprotent — network protocol database functions
// struct protoent *getprotobyname(const char *name);
// struct protoent *getprotobynumber(int proto);
// struct protoent *getprotoent(void);

// fcntl - manipulate file descriptor
// int fcntl(int fd, int op, ... /* arg */ );