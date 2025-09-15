#include "HttpRequest.hpp"

HttpRequest::HttpRequest() : _method(UNKNOWN), _uri("") {
	console::log("[HttpRequest Default Constructor]", DEBUG);
}

HttpRequest::HttpRequest(const HttpRequest& rhs) : HttpMessage(rhs), _method(rhs._method), _uri(rhs._uri) {
	console::log("[HttpRequest Copy Constructor]", DEBUG);
}

HttpRequest& HttpRequest::operator=(const HttpRequest& rhs) {
	console::log("[HttpRequest Assignment Operator]", DEBUG);
	if (this != &rhs) {
		HttpMessage::operator=(rhs);
		_method = rhs._method;
		_uri = rhs._uri;
	}
	return *this;
}

HttpRequest::~HttpRequest() {
	console::log("[HttpRequest Destructor]", DEBUG);
}

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
