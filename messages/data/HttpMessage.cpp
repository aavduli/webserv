#include "HttpMessage.hpp"

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

bool	HttpMessage::hasHeader(const std::string& key) const {
	std::string normalized = toLowerStr(key);
	return _headers.find(normalized) != _headers.end();
}

void	HttpMessage::addHeader(const std::string& key, const std::string& value) {

	(void)key;
	(void)value;

	std::string					new_key;	// TODO PARSING -> extract key from string
	std::vector<std::string>	new_value;	// TODO PARSING -> extract value from string

	// TODO PARSING -> check for multiple comma-separated strings in new_value

	if (this->hasHeader(new_key)) {		// if key duplicate, check for value uniqueness 
		std::vector<std::string>::iterator	new_it;
		for (new_it = new_value.begin(); new_it != new_value.end(); ++new_it) {
			std::vector<std::string>::iterator	it;
			for (it = _headers.at(new_key).begin(); it != _headers.at(new_key).end(); ++it) {
				if (*it == *new_it)
					return ;							// if duplicate, exit without adding
			}
			_headers.at(new_key).push_back(*new_it);		// if unique, add value to vector
		}
	}
	else {
		_headers[new_key] = new_value;	// if unique, add new key-value pair
	}
}

// TODO -> add checks
std::vector<std::string>	HttpMessage::getHeaderValues(const std::string& key) const {
	
	std::string normalized = toLowerStr(key);
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
