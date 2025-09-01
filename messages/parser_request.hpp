#ifndef PARSER_REQUEST_HPP
#define PARSER_REQUEST_HPP

#include "parser_message.hpp"

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

class HttpRequest : public HttpMessage {

	private:
		HttpMethod	method;
		std::string	uri;		// needs to be a class

	public:
		HttpRequest();
		HttpRequest(const HttpRequest& rhs);
		HttpRequest& operator=(const HttpRequest& rhs);
		~HttpRequest();

};

#endif // PARSER_REQUEST_HPP