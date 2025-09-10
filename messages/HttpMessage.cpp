#include "HttpMessage.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <iostream>
#include <sstream>

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

void	handle_message(s_MessageStreams *streams) {

	// tmp
	// 	std::stringstream ss;
	// 	ss << streams.simple.rdbuf();
	// 	std::string	raw_request = ss.str();
	// 
	// 	HttpRequest request(raw_request);

	HttpRequest		request;
	HttpResponse	response;
	std::fstream	*stream;

	// 1. Decide which stream to read from
	if (streams->has_eof && !streams->is_chunked)
		stream = &streams->simple;
	else if (streams->is_chunked)
		stream = &streams->secondary;

	switch (request.getState()) {
		case s_msg_dead:
			break;
		case s_msg_empty:
			break;
		case s_msg_error:
			break;
		case s_msg_init:
			break;
		case s_msg_version:
			break;
		case s_req_start:
			break;
		case s_req_method:
			break;
		case s_req_uri:
			break;
		case s_req_done:
			break;
		case s_head_start:
			break;
		case s_head_fields:
			break;
		case s_head_done:
			break;
		case s_body_start:
			break;
		case s_body_content:
			break;
		case s_body_done:
			break;
		case s_msg_done:
			break;
	}
	
	std::string stream_line;

		stream_line = get_stream_line(streams->simple);
		stream_line = get_stream_line(streams->secondary);

	
	get_chunked_request(*streams);
	get_simple_request(*streams);
		
	// with sta	te checking and switch case
	// extract line by line to parse content
	// create Request object
	// handle request
	// create response
	// populate response stream

}

std::string	get_stream_line(std::fstream& stream) {
	// getline
	return "";
}
