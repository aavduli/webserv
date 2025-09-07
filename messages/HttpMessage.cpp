#include "HttpMessage.hpp"
#include <iostream>

HttpMessage::HttpMessage() : _state(s_msg_init), _http_version(-1), _headers(), _body() {
	std::cout << "[HttpMessage Default Constructor]" << std::endl;
}

HttpMessage::HttpMessage(const HttpMessage& rhs) : _state(rhs._state), _http_version(rhs._http_version), _headers(rhs._headers), _body(rhs._body) {
	std::cout << "[HttpMessage Copy Constructor]" << std::endl;
}

HttpMessage& HttpMessage::operator=(const HttpMessage& rhs) {
	std::cout << "[HttpMessage Assignment Operator]" << std::endl;
	if (this != &rhs) {
		_state = rhs._state;
		_http_version = rhs._http_version;
		_headers = rhs._headers;
		_body = rhs._body;
	}
	return *this;
}

HttpMessage::~HttpMessage() {
	std::cout << "[HttpMessage Destructor]" << std::endl;
}

State	HttpMessage::getState() const {
	return _state;
}

void	HttpMessage::setState(State state) {
	_state = state;
}

double	HttpMessage::getHttpVersion() const {
	return _http_version;
}

void	HttpMessage::setHttpVersion(double version) {
	_http_version = version;
}

HttpHeaders	HttpMessage::getHeaders() const {
	return _headers;
}

void	HttpMessage::setHeaders(const std::map<std::string, std::vector<std::string> >& fields) {
	// TODO PARSING -> fields parsed from buffer
	_headers = HttpHeaders(fields);
}

std::string	HttpMessage::getBody() const {
	return _body;
}

void	HttpMessage::setBody(const std::string& body) {
	_body = body;
}
