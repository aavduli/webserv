#include "HttpRequest.hpp"
#include <iostream>

HttpRequest::HttpRequest(char* buffer) {
	std::cout << "[HttpRequest Parameterized Constructor]" << std::endl;
	(void)buffer;
}

HttpRequest::HttpRequest(const HttpRequest& rhs) : HttpMessage(rhs), _method(rhs._method), _uri(rhs._uri) {
	std::cout << "[HttpRequest Copy Constructor]" << std::endl;
}

HttpRequest& HttpRequest::operator=(const HttpRequest& rhs) {
	std::cout << "[HttpRequest Assignment Operator]" << std::endl;
	if (this != &rhs) {
		HttpMessage::operator=(rhs);
		_method = rhs._method;
		_uri = rhs._uri;
	}
	return *this;
}

HttpRequest::~HttpRequest() {
	std::cout << "[HttpRequest Destructor]" << std::endl;
}