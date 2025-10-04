#include "RequestUri.hpp"
#include "../parsing/MessageParser.hpp"

RequestUri::RequestUri() : _raw_uri(""), _scheme(""), _userinfo(""), _host(""), _port(""), _path(""),
	_effective_path(""), _redir_destination(""), _query(""), _fragment(""), _is_absolute_uri(false), _is_abs_path(false) {}

RequestUri::RequestUri(const std::string& raw_uri) : _raw_uri(raw_uri), _scheme(""), _userinfo(""), _host(""), _port(""),
	_path(""), _effective_path(""), _redir_destination(""), _query(""), _fragment(""), _is_absolute_uri(false), _is_abs_path(false) {
}

RequestUri::RequestUri(const RequestUri& rhs) : _raw_uri(rhs._raw_uri), _scheme(rhs._scheme),
	_userinfo(rhs._userinfo), _host(rhs._host), _port(rhs._port), _path(rhs._path), _effective_path(rhs._effective_path),  _redir_destination(""), 
	_query(rhs._query), _fragment(rhs._fragment), _is_absolute_uri(rhs._is_absolute_uri), _is_abs_path(rhs._is_abs_path) {}

RequestUri& RequestUri::operator=(const RequestUri& rhs) {
	if (this != &rhs) {
		_raw_uri = rhs._raw_uri;
		_scheme = rhs._scheme;
		_userinfo = rhs._userinfo;
		_host = rhs._host;
		_port = rhs._port;
		_path = rhs._path;
		_effective_path = rhs._effective_path;
		_redir_destination = rhs._redir_destination;
		_query = rhs._query;
		_fragment = rhs._fragment;
		_is_absolute_uri = rhs._is_absolute_uri;
		_is_abs_path = rhs._is_abs_path;
	}
	return *this;
}

RequestUri::~RequestUri() {}

bool	RequestUri::parse() {

	clearUri();

	// The Request-URI is transmitted as an encoded string, where some
	// characters may be escaped using the "% HEX HEX" encoding defined by
	// RFC 1738 [4]. The origin server must decode the Request-URI in order
	// to properly interpret the request.
	// decode_uri();

	if (_raw_uri.empty())
		_raw_uri = "/";		// If the abs_path is not present in the URL, it MUST be given as "/"
	else if (isAbsoluteUri(_raw_uri))
		*this = parseAbsoluteUri(_raw_uri);
	else if (_raw_uri[0] == '/')
		*this = parseAbsolutePath(_raw_uri);
	else {
		console::log("[ERROR] Invalid URI format: " + _raw_uri, MSG);
		return false;
	}

	// encode_uri(); ?

	return true;
}

bool	RequestUri::parseUriPath(const std::string& raw) {

	size_t pos = 0;
	
	if (raw.find("?", pos) != std::string::npos) {
		_path = extractComponent(&pos, raw, "?");
		_query = raw.substr(pos);
	}
	else {
		_path = raw.substr(pos);
		_query = "";
	}
	return true;
}

bool	RequestUri::parseUriAuthority(const std::string& raw) {

	size_t pos = 0;
	
	if (raw.find("@") != std::string::npos)
		_userinfo = extractComponent(&pos, raw, "@");

	if (raw.find(":", pos) != std::string::npos && raw.find(":", pos) > pos) {
		_host = extractComponent(&pos, raw, ":");
		_port = raw.substr(pos);
	}
	else {
		_host = raw.substr(pos);
		_port = "80";
	}
	return true;
}

RequestUri	RequestUri::parseAbsoluteUri(const std::string& raw) {

	RequestUri uri;
	size_t pos = 0;
	uri._raw_uri = raw;
	uri._is_absolute_uri = true;

	if (raw.find(":") != std::string::npos) {
		uri._scheme = extractComponent(&pos, raw, ":");
		if (uri._scheme != "http" && uri._scheme != "https") {
			console::log("[ERROR] Unsupported URI scheme: " + uri._scheme, MSG);
			uri.clearUri();
			return uri;
		}
		if (pos < raw.length() && raw.substr(pos, 2) == "//") {
			pos += 2;
			size_t authority_end = raw.find_first_of("/?", pos);
			if (authority_end == std::string::npos)
				authority_end = raw.length();
			std::string authority = raw.substr(pos, authority_end - pos);
			uri.parseUriAuthority(authority);
			pos = authority_end;
		}
		if (pos < raw.length())
			uri.parseUriPath(raw.substr(pos));
		else
			uri._path = "/";
		uri._is_absolute_uri = true;
	}
	return uri;
}

RequestUri	RequestUri::parseAbsolutePath(const std::string& raw) {

	RequestUri uri;
	uri._raw_uri = raw;
	uri._is_abs_path = true;
	uri.parseUriPath(raw);
	return uri;
}

// UTILS

std::string RequestUri::extractComponent(size_t* pos, const std::string& str, const std::string& del) {

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

bool RequestUri::isAbsoluteUri(const std::string& uri) {

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

void RequestUri::clearUri() {
	_scheme = "";
	_userinfo = "";
	_host = "";
	_port = "";
	_path = "";
	_effective_path = "";
	_redir_destination = "";
	_query = "";
	_fragment = "";
	_is_absolute_uri = false;
	_is_abs_path = false;
}

void	RequestUri::print() const {

	std::cout << "RequestUri Debug Info:" << std::endl;
	std::cout << "  Raw URI: " << _raw_uri << std::endl;
	std::cout << "  Scheme: " << _scheme << std::endl;
	std::cout << "  Userinfo: " << _userinfo << std::endl;
	std::cout << "  Host: " << _host << std::endl;
	std::cout << "  Port: " << _port << std::endl;
	std::cout << "  Path: " << _path << std::endl;
	std::cout << "  Effective path: " << _effective_path << std::endl;
	std::cout << "  Redirection destination: " << _redir_destination << std::endl;
	std::cout << "  Query: " << _query << std::endl;
	std::cout << "  Fragment: " << _fragment << std::endl;
	std::cout << "  Is Absolute URI: " << (_is_absolute_uri ? "true" : "false") << std::endl;
	std::cout << "  Is Abs Path: " << (_is_abs_path ? "true" : "false") << std::endl;
}

// ACCESSORS

std::string	RequestUri::getRawUri() const {
	return _raw_uri;
}

void	RequestUri::setRawUri(const std::string& raw) {
	_raw_uri = raw;
}

std::string	RequestUri::getPath() const {
	return _path;
}

std::string	RequestUri::getEffectivePath() const {
	return _effective_path;
}

std::string	RequestUri::getRedirDestination() const {
	return _redir_destination;
}

// Additional Getters
std::string	RequestUri::getScheme() const {
	return _scheme;
}

std::string	RequestUri::getUserinfo() const {
	return _userinfo;
}

std::string	RequestUri::getHost() const {
	return _host;
}

std::string	RequestUri::getPort() const {
	return _port;
}

std::string	RequestUri::getQuery() const {
	return _query;
}

bool	RequestUri::isAbsoluteUri() const {
	return _is_absolute_uri;
}

bool	RequestUri::isAbsolutePath() const {
	return _is_abs_path;
}

// Additional Setters
void	RequestUri::setScheme(const std::string& scheme) {
	_scheme = scheme;
}

void	RequestUri::setUserinfo(const std::string& userinfo) {
	_userinfo = userinfo;
}

void	RequestUri::setHost(const std::string& host) {
	_host = host;
}

void	RequestUri::setPort(const std::string& port) {
	_port = port;
}

void	RequestUri::setPath(const std::string& path) {
	_path = path;
}

void	RequestUri::setEffectivePath(const std::string& full_path) {
	_effective_path = full_path;
}

void	RequestUri::setRedirDestination(const std::string& destination) {
	_redir_destination = destination;
}

void	RequestUri::setQuery(const std::string& query) {
	_query = query;
}

void	RequestUri::setFragment(const std::string& fragment) {
	_fragment = fragment;
}

void	RequestUri::setAbsoluteUri(bool is_absolute_uri) {
	_is_absolute_uri = is_absolute_uri;
}

void	RequestUri::setAbsPath(bool is_abs_path) {
	_is_abs_path = is_abs_path;
}
