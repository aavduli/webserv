#include "RequestUri.hpp"
#include "../parsing/MessageParser.hpp"

// Constructors
RequestUri::RequestUri() : _raw_uri(""), _scheme(""), _userinfo(""), _host(""), _port(""), _path(""),
	_query(""), _fragment(""), _is_valid_uri(false), _is_absolute_uri(false), _is_abs_path(false) {}

RequestUri::RequestUri(const std::string& raw_uri) : _raw_uri(raw_uri), _scheme(""), _userinfo(""), _host(""), _port(""),
	_path(""), _query(""), _fragment(""), _is_valid_uri(false), _is_absolute_uri(false), _is_abs_path(false) {
	parse(raw_uri);
}

RequestUri::RequestUri(const RequestUri& rhs) : _raw_uri(rhs._raw_uri), _scheme(rhs._scheme),
	_userinfo(rhs._userinfo), _host(rhs._host), _port(rhs._port), _path(rhs._path),
	_query(rhs._query), _fragment(rhs._fragment), _is_valid_uri(rhs._is_valid_uri),
	_is_absolute_uri(rhs._is_absolute_uri), _is_abs_path(rhs._is_abs_path) {}

RequestUri& RequestUri::operator=(const RequestUri& rhs) {
	if (this != &rhs) {
		_raw_uri = rhs._raw_uri;
		_scheme = rhs._scheme;
		_userinfo = rhs._userinfo;
		_host = rhs._host;
		_port = rhs._port;
		_path = rhs._path;
		_query = rhs._query;
		_fragment = rhs._fragment;
		_is_valid_uri = rhs._is_valid_uri;
		_is_absolute_uri = rhs._is_absolute_uri;
		_is_abs_path = rhs._is_abs_path;
	}
	return *this;
}

RequestUri::~RequestUri() {}

/*
TODO: validate URI against config
TODO: URL decode special characters (%20, etc.)
TODO: Validate against malicious paths (../, etc.)
*/

bool	RequestUri::parse(const std::string& raw_uri) {

	_raw_uri = trim_whitespaces(raw_uri);
	if (_raw_uri.empty()) {
		console::log("Empty request URI", ERROR, ALL);
		return false;
	}
	if (_raw_uri.length() > MAX_URI_LENGTH) {
		console::log("Request URI too long", ERROR, ALL);
		return false;
	}
	clear_uri();
	if (is_absolute_uri(_raw_uri)) {
		*this = parse_absolute_uri(_raw_uri);
	}
	else if (_raw_uri[0] == '/')
		*this = parse_abs_path(_raw_uri);
	else {
		console::log("Invalid URI format", ERROR, ALL);
		return false;
	}
	_is_valid_uri = true;
	return _is_valid_uri;
}

std::string RequestUri::extract_uri_component(size_t* pos, const std::string& str, const std::string& del) {

	size_t component_pos = *pos;
	*pos = str.find(del, component_pos);
	if (*pos == std::string::npos) {
		std::string component = str.substr(component_pos);
		*pos = str.length();
		return component;
	}
	std::string component = str.substr(component_pos, *pos - component_pos);
	*pos += del.size();
	return component;
}

bool RequestUri::is_absolute_uri(const std::string& uri) {

	size_t colon_pos = uri.find(":");
	if (colon_pos == std::string::npos || colon_pos == 0)
		return false;
	
	std::string scheme = uri.substr(0, colon_pos);
	for (size_t i = 0; i < scheme.length(); i++) {
		if (!isalpha(scheme[i]) && !isdigit(scheme[i]) && scheme[i] != '+' && scheme[i] != '-' && scheme[i] != '.')
			return false;
	}
	if (!isalpha(scheme[0]))
		return false;
	return true;
}

void RequestUri::clear_uri() {

	_scheme = "";
	_userinfo = "";
	_host = "";
	_port = "";
	_path = "";
	_query = "";
	_fragment = "";
	_is_valid_uri = false;
	_is_absolute_uri = false;
	_is_abs_path = false;
}

bool	RequestUri::parse_uri_path_query(const std::string& raw) {

	size_t pos = 0;
	
	if (raw.find("?", pos) != std::string::npos) {
		_path = extract_uri_component(&pos, raw, "?");
		_query = raw.substr(pos);
	}
	else {
		_path = raw.substr(pos);
		_query = "";
	}
	return true;
}

bool	RequestUri::parse_uri_authority(const std::string& raw) {

	size_t pos = 0;
	
	if (raw.find("@") != std::string::npos)
		_userinfo = extract_uri_component(&pos, raw, "@");

	if (raw.find(":", pos) != std::string::npos && raw.find(":", pos) > pos) {
		_host = extract_uri_component(&pos, raw, ":");
		_port = raw.substr(pos);
	}
	else {
		_host = raw.substr(pos);
		_port = "80";	// get default from server config
	}
	return true;
}

RequestUri	RequestUri::parse_absolute_uri(const std::string& raw) {

	RequestUri uri;
	size_t pos = 0;
	uri._raw_uri = raw;
	uri._is_absolute_uri = true;

	if (raw.find(":") != std::string::npos) {
		uri._scheme = extract_uri_component(&pos, raw, ":");
		if (uri._scheme != "http" && uri._scheme != "https") {
			console::log("Unsupported URI scheme: " + uri._scheme, ERROR, ALL);
			uri.clear_uri();
			return uri;
		}
		if (pos < raw.length() && raw.substr(pos, 2) == "//") {
			pos += 2;
			size_t authority_end = raw.find_first_of("/?", pos);
			if (authority_end == std::string::npos)
				authority_end = raw.length();
			std::string authority = raw.substr(pos, authority_end - pos);
			uri.parse_uri_authority(authority);
			pos = authority_end;
		}
		if (pos < raw.length())
			uri.parse_uri_path_query(raw.substr(pos));
		else
			uri._path = "/";
	}
	return uri;
}

RequestUri	RequestUri::parse_abs_path(const std::string& raw) {

	RequestUri uri;
	uri._raw_uri = raw;
	uri._is_abs_path = true;
	uri.parse_uri_path_query(raw);
	return uri;
}

void	RequestUri::print() const {

	std::cout << "RequestUri Debug Info:" << std::endl;
	std::cout << "  Raw URI: " << _raw_uri << std::endl;
	std::cout << "  Scheme: " << _scheme << std::endl;
	std::cout << "  Userinfo: " << _userinfo << std::endl;
	std::cout << "  Host: " << _host << std::endl;
	std::cout << "  Port: " << _port << std::endl;
	std::cout << "  Path: " << _path << std::endl;
	std::cout << "  Query: " << _query << std::endl;
	std::cout << "  Fragment: " << _fragment << std::endl;
	std::cout << "  Is Valid: " << (_is_valid_uri ? "true" : "false") << std::endl;
	std::cout << "  Is Absolute URI: " << (_is_absolute_uri ? "true" : "false") << std::endl;
	std::cout << "  Is Abs Path: " << (_is_abs_path ? "true" : "false") << std::endl;
}

std::string	RequestUri::getRawUri() const {
	return _raw_uri;
}
