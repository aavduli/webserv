#include "HttpHeaders.hpp"
#include <iostream>

HttpHeaders::HttpHeaders() : _fields() {
	std::cout << "[HttpHeaders Default Constructor]" << std::endl;
}

HttpHeaders::HttpHeaders(const std::map<std::string, std::vector<std::string> >& fields) : _fields(fields) {
	std::cout << "[HttpHeaders Parameterized Constructor]" << std::endl;
}

HttpHeaders::HttpHeaders(const HttpHeaders& rhs) : _fields(rhs._fields) {
	std::cout << "[HttpHeaders Copy Constructor]" << std::endl;
}

HttpHeaders& HttpHeaders::operator=(const HttpHeaders& rhs) {
	std::cout << "[HttpHeaders Assignment Operator]" << std::endl;
	if (this != &rhs) {
		_fields = rhs._fields;
	}
	return *this;
}

HttpHeaders::~HttpHeaders() {
	std::cout << "[HttpHeaders Destructor]" << std::endl;
}

bool	HttpHeaders::hasHeader(const std::string& key) const {
	std::string normalized = toLowerStr(key);
	return _fields.find(normalized) != _fields.end();
}

void	HttpHeaders::addHeader(const std::string& key, const std::string& value) {

	(void)key;
	(void)value;

	std::string					new_key;	// TODO PARSING -> extract key from string
	std::vector<std::string>	new_value;	// TODO PARSING -> extract value from string

	// TODO PARSING -> check for multiple comma-separated strings in new_value

	if (this->hasHeader(new_key)) {		// if key duplicate, check for value uniqueness 
		std::vector<std::string>::iterator	new_it;
		for (new_it = new_value.begin(); new_it != new_value.end(); ++new_it) {
			std::vector<std::string>::iterator	it;
			for (it = _fields.at(new_key).begin(); it != _fields.at(new_key).end(); ++it) {
				if (*it == *new_it)
					return ;							// if duplicate, exit without adding
			}
			_fields.at(new_key).push_back(*new_it);		// if unique, add value to vector
		}
	}
	else {
		_fields[new_key] = new_value;	// if unique, add new key-value pair
	}
}

// TODO -> add checks
std::vector<std::string>	HttpHeaders::getHeaderValues(const std::string& key) const {
	
	std::string normalized = toLowerStr(key);
	if (this->hasHeader(normalized)) {
		std::map<std::string, std::vector<std::string> >::const_iterator it;
		it = _fields.find(normalized);
		if (it != _fields.end())
			return it->second;
	}
	return std::vector<std::string>();
}

// TODO
void	HttpHeaders::setHeaderValues(const std::string& key, const std::string& value) {
	(void)key;
	(void)value;
}

