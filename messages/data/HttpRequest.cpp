#include "HttpRequest.hpp"

HttpRequest::HttpRequest() : _method(""), _uri("") {
	std::cout << "[HttpRequest Default Constructor]" << std::endl;
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

std::string HttpRequest::getMethod() const {
	return _method;
}

std::string HttpRequest::getUri() const {
	return _uri;
}
