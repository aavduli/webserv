#include "HttpRequest.hpp"

HttpRequest::HttpRequest(char *buffer, ssize_t size) {
	std::cout << "[HttpRequest Parameterized Constructor]" << std::endl;

	std::string	raw_request(buffer, size);
	if (!parse_request(raw_request))
		std::cout << "An error occured during request parsing" << std::endl;
	else {
		std::cout << "Request parsing successfull" << std::endl;
		//print_request(*this);
	}
	return ;
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

// separate parsing in steps and monitor with state
bool HttpRequest::parse_request(std::string raw_request) {
	
	if (raw_request.empty()) {
		_state = s_msg_empty;
		std::cerr << "raw_request empty" << std::endl;
		return false;
	}

	_state = s_req_start;
	std::string::const_iterator it = raw_request.begin();
	std::string::const_iterator end = raw_request.end();
	
	while (it != end) {
		//std::cout << "current char [" << *it << "] state: " << _state << std::endl;
		switch (_state) {
			case s_req_start:
				if (!parse_req_method(it, end))
					return false;
				break;
			case s_req_method:
				if (!parse_req_uri(it, end))
					return false;
				break;
			case s_req_uri:
				if (!parse_req_version(it, end))
					return false;
				break;
			case s_req_version:
				if (!parse_req_headers(it, end))
					return false;
				break;
			case s_head_done:
				if (!parse_req_body(it, end))
					return false;
				break;
			case s_body_done:
				if (!is_valid_request())
					return false;
				break;
			case s_req_done:
				return true;
			default:
				std::cerr << "Unfinished/unknown parsing state" << std::endl;
				return false;
		}
	}

	// if it != end, chars remaining
	// Skip to end of line
	// while (it != end && *it != '\n')
	// 	++it;
	// Skip the \n
	// if (it != end && *it == '\n')
	// 	++it;

	return (_state == s_req_done);
}

// Parse "METHOD URI HTTP/VERSION\r\n"
// Example: "GET /index.html HTTP/1.1\r\n"
/* 
The Request-Line begins with a method token, followed by the
   Request-URI and the protocol version, and ending with CRLF. The
   elements are separated by SP characters. No CR or LF is allowed
   except in the final CRLF sequence.

        Request-Line   = Method SP Request-URI SP HTTP-Version CRLF
 */

bool	HttpRequest::parse_req_version(std::string::const_iterator& it, std::string::const_iterator end) {

	// Skip SP between URI and http version
	while (it != end && (*it == ' ' || *it == '\t'))
		++it;

	std::string version;
	while (it != end && *it != ' ' && *it != '\t' && *it != '\r' && *it != '\n') {
		version += *it;
		++it;
	}
	if (version.empty()) {
		std::cerr << "No HTTP version found" << std::endl;
		return false;
	}
	else {

		std::cout << "HTTP version:	" << version << std::endl;
		_state = s_req_version;
		return true;
	}
}

bool	HttpRequest::parse_req_uri(std::string::const_iterator& it, std::string::const_iterator end) {

	// Skip SP between method and URI
	while (it != end && (*it == ' ' || *it == '\t'))
		++it;

	std::string uri;
	while (it != end && *it != ' ' && *it != '\t' && *it != '\r' && *it != '\n') {
		uri += *it;
		++it;
	}
	if (uri.empty()) {
		std::cerr << "No URI found" << std::endl;
		return false;
	}
	else {
		_uri = uri;
		std::cout << "URI:		" << _uri << std::endl;
		_state = s_req_uri;
		return true;
	}
}

bool	HttpRequest::parse_req_method(std::string::const_iterator& it, std::string::const_iterator end) {

	// Skip LWS
	while (it != end && (*it == ' ' || *it == '\t'))
		++it;

	std::string method;
	while (it != end && *it != ' ' && *it != '\t' && *it != '\r' && *it != '\n') {
		method += *it;
		++it;
	}
	if (method.empty()) {
		std::cerr << "No HTTP method found" << std::endl;
		return false;
	}
	else {
		_method = method;
		std::cout << "Method:		" << _method << std::endl;
		_state = s_req_method;
		return true;
	}
}

// TODO: Implement header parsing
// For now, just skip to body or end
bool HttpRequest::parse_req_headers(std::string::const_iterator& it, std::string::const_iterator end) {
	std::cout << "Parsing headers (TODO)" << std::endl;
	_state = s_head_done;
	(void)it;
	(void)end;
	return true;
}

// TODO: Implement body parsing based on Content-Length or Transfer-Encoding
bool HttpRequest::parse_req_body(std::string::const_iterator& it, std::string::const_iterator end) {
	std::cout << "Parsing body (TODO)" << std::endl;
	(void)it;
	(void)end;
	_state = s_body_done;
	return true;
}

bool	HttpRequest::is_valid_request() {
	// check against server config
	_state = s_req_done;
	return true;
}
