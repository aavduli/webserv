#include "HttpRequest.hpp"

HttpRequest::HttpRequest() : _method(UNKNOWN), _uri("") {}

HttpRequest::HttpRequest(const HttpRequest& rhs) : HttpMessage(rhs), _method(rhs._method), _uri(rhs._uri) {}

HttpRequest& HttpRequest::operator=(const HttpRequest& rhs) {
	if (this != &rhs) {
		HttpMessage::operator=(rhs);
		_method = rhs._method;
		_uri = rhs._uri;
	}
	return *this;
}

HttpRequest::~HttpRequest() {}

HttpMethod HttpRequest::getMethod() const {
	return _method;
}

void	HttpRequest::setMethod(HttpMethod method) {
	_method = method;
}

std::string HttpRequest::getUri() const {
	return _uri;
}

void	HttpRequest::setUri(std::string uri) {
	_uri = uri;
}

void	print_request(HttpRequest* request) {
	std::cout << "\n=== HTTP REQUEST ===" << std::endl;
	
	// Print request line
	std::cout << "Method:		" << request->getMethod() << std::endl;
	std::cout << "URI:		" << request->getUri() << std::endl;
	std::cout << "HTTP Version:	" << request->getHttpVersion() << std::endl;
	std::cout << "Headers:" << std::endl;
	request->printHeaders();
	std::string body = request->getBody();
	if (!body.empty()) {
		std::cout << "Body (" << body.length() << " bytes):" << std::endl;
		std::cout << body << std::endl;
	} else {
		std::cout << "Body: (empty)" << std::endl;
	}
	std::cout << "===================" << std::endl;
}