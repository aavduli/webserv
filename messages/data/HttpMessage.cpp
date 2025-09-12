#include "HttpMessage.hpp"
#include "HttpRequest.hpp"

HttpMessage::HttpMessage() {
	std::cout << "[HttpMessage Default Constructor]" << std::endl;
	_state = s_msg_dead;
	_http_version = "";
	_body = "";
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

std::string	HttpMessage::getHttpVersion() const {
	return _http_version;
}

void	HttpMessage::setHttpVersion(std::string version) {
	_http_version = version;
}

bool	HttpMessage::hasHeader(const std::string& key) const {
	std::string normalized = lower(key);
	return _headers.find(normalized) != _headers.end();
}

void	HttpMessage::addHeader(const std::string& key, const std::vector<std::string>& values) {

	if (this->hasHeader(key)) {		// if key duplicate, check for value uniqueness 
		std::vector<std::string>::const_iterator	new_it;
		for (new_it = values.begin(); new_it != values.end(); ++new_it) {
			std::vector<std::string>::iterator	it;
			for (it = _headers.at(key).begin(); it != _headers.at(key).end(); ++it) {
				if (*it == *new_it)
					return ;							// if duplicate, exit without adding
			}
			_headers.at(key).push_back(*new_it);		// if unique, add value to vector
		}
	}
	else {
		_headers[lower(key)] = values;	// if unique, add new key-value pair
	}
}

// TODO -> add checks
std::vector<std::string>	HttpMessage::getHeaderValues(const std::string& key) const {
	
	std::string normalized = lower(key);
	if (this->hasHeader(normalized)) {
		std::map<std::string, std::vector<std::string> >::const_iterator it;
		it = _headers.find(normalized);
		if (it != _headers.end())
			return it->second;
	}
	return std::vector<std::string>();
}

// TODO
void	HttpMessage::setHeaderValues(const std::string& key, const std::string& value) {
	(void)key;
	(void)value;
}

std::string	HttpMessage::getBody() const {
	return _body;
}

void	HttpMessage::setBody(const std::string& body) {
	_body = body;
}

const char* get_response(std::string raw_request) {

	std::cout << "raw_request received: \n" << raw_request << std::endl;
	if (raw_request.empty()) {
		console::log("empty request", WARNING);
		return NULL;
	}
	else {
		RequestParser	parser;
		HttpRequest*	request = parser.parse_request(raw_request);
		if (request)
			print_request(request);
	}

	return "temporary response\n";
}
