#include "HttpMessage.hpp"
#include "HttpRequest.hpp"

HttpMessage::HttpMessage() {
	_version_major = "";
	_version_minor = "";
	_body = "";
	_body = E_INIT;
}
HttpMessage::HttpMessage(const HttpMessage& rhs) : _version_major(rhs._version_major), _version_minor(rhs._version_minor), _headers(rhs._headers), _body(rhs._body), _body_type(rhs._body_type) {}
HttpMessage& HttpMessage::operator=(const HttpMessage& rhs) {
	if (this != &rhs) {
		_version_major = rhs._version_major;
		_version_minor = rhs._version_minor;
		_headers = rhs._headers;
		_body = rhs._body;
		_body_type = rhs._body_type;
	}
	return *this;
}
HttpMessage::~HttpMessage() {}

std::string	HttpMessage::getHttpVersion() const {return (_version_major + "." + _version_minor);}
std::string	HttpMessage::getHttpVersionMajor() const {return _version_major;}
std::string	HttpMessage::getHttpVersionMinor() const {return _version_minor;}

void	HttpMessage::setHttpVersion(std::string major, std::string minor) {
	_version_major = major;
	_version_minor = minor;
}

bool	HttpMessage::hasHeader(const std::string& key) const {
	std::string normalized = lower(key);
	return _headers.find(normalized) != _headers.end();
}

// includes keys and values uniqueness check
void	HttpMessage::addHeader(const std::string& key, const std::vector<std::string>& values) {

	if (this->hasHeader(key)) { 
		std::vector<std::string>::const_iterator	new_it;
		for (new_it = values.begin(); new_it != values.end(); ++new_it) {
			std::vector<std::string>::iterator	it;
			for (it = _headers.at(key).begin(); it != _headers.at(key).end(); ++it) {
				if (*it == *new_it)
					return ;
			}
			_headers.at(key).push_back(trim_lws(*new_it));
		}
	}
	else
		_headers[lower(key)] = values;
}

std::map<std::string, std::vector<std::string> > HttpMessage::getHeaders() const {return _headers;}

const std::vector<std::string>&	HttpMessage::getHeaderValues(const std::string& key) const {
	
	std::string normalized = lower(key);
	if (this->hasHeader(normalized)) {
		std::map<std::string, std::vector<std::string> >::const_iterator it;
		it = _headers.find(normalized);
		if (it != _headers.end())
			return it->second;
	}
	static const std::vector<std::string> empty;
	return empty;
}

void	HttpMessage::printHeaders() const {

	std::map<std::string, std::vector<std::string> >::const_iterator	map_it;
	std::vector<std::string>::const_iterator	vector_it;

	for (map_it = _headers.begin(); map_it != _headers.end(); map_it++) {
		std::cout << map_it->first << ": ";
		for (vector_it = map_it->second.begin(); vector_it != map_it->second.end(); vector_it++)
			std::cout << *vector_it << " / ";
		std::cout << std::endl;
	}
}

size_t	HttpMessage::getHeadersSize() const {return _headers_size;}
void	HttpMessage::setHeadersSize(size_t headers_size) {_headers_size = headers_size;}
std::string	HttpMessage::getBody() const {return _body;}
void	HttpMessage::setBody(const std::string& body) {_body = body;}
BodyType	HttpMessage::getBodyType() const {return _body_type;}
void	HttpMessage::setBodyType(BodyType body_type) {_body_type = body_type;}