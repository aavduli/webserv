#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include "HttpMessage.hpp"

enum HttpMethod {
	GET,		// 0
	POST,		// 1
	DELETE,		// 2
	HEAD,		// 3
	PUT,		// 4
	CONNECT,	// 5
	OPTIONS,	// 6
	TRACE,		// 7
};

/* 
Request read in buffer:
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	ssize_t bytes_read = read(connection, buffer, sizeof(buffer) - 1);

TODO: Parse incoming HTTP request from buffer
	HttpRequest request(buffer);

TODO: Create HTTP response with status code, headers and output body if needed
	HttpResponse response(request);
 */

class HttpRequest : public HttpMessage {

	private:
		HttpMethod	_method;
		std::string	_uri;		// needs to be a class

	public:
		HttpRequest(char* buffer);
		HttpRequest(const HttpRequest& rhs);
		HttpRequest& operator=(const HttpRequest& rhs);
		~HttpRequest();
};

#endif // HTTPREQUEST_HPP