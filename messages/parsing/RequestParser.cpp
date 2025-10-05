#include "RequestParser.hpp"

RequestParser::RequestParser(HttpRequest* request, const std::string& raw_request) : _request(request), _raw_request(raw_request), _current_pos(0), _last_status(E_INIT) {}
RequestParser::RequestParser(const RequestParser& rhs) : _request(rhs._request), _raw_request(rhs._raw_request), _current_pos(rhs._current_pos), _last_status(rhs._last_status) {}
RequestParser& RequestParser::operator=(const RequestParser& rhs) {
	if (this != &rhs) {
		_request = rhs._request;
		_raw_request = rhs._raw_request;
		_current_pos = rhs._current_pos;
		_last_status = rhs._last_status;
	}
	return *this;
}
RequestParser::~RequestParser() {}
Status RequestParser::getLastStatus() const {return _last_status;}

bool RequestParser::parseRequest() {

	if (!parseRequestLine()) {
		console::log("[ERROR] Failed to parse request line", MSG);
		return false;
	}
	if (!parseHeaders()) {
		console::log("[ERROR] Failed to parse headers", MSG);
		return false;
	}
	if (!parseBody()) {
		console::log("[ERROR] Failed to parse body", MSG);
		return false;
	}
	console::log("[INFO] Request parsing success", MSG);
	return true;
}

bool RequestParser::parseRequestLine() {

	size_t line_end = _raw_request.find("\r\n", _current_pos);
	if (line_end == std::string::npos) {
		console::log("No CRLF found in request line", ERROR);
		return false;
	}
	std::string request_line = _raw_request.substr(_current_pos, line_end - _current_pos);
	request_line = trim_lws(request_line);
	
	if (!parseMethod(request_line))
		return false;
	if (!parseUri(request_line))
		return false;
	if (!parseVersion(request_line))
		return false;
	_current_pos = line_end + 2;
	return true;
}

bool RequestParser::parseMethod(std::string request_line) {

	size_t method_end = request_line.find(" ", _current_pos);
	if (method_end == std::string::npos) {
		console::log("[ERROR] No SP found after method", MSG);
		// 501 Not Implemented
		return false;
	}
	std::string method = request_line.substr(_current_pos, method_end - _current_pos);
	if (method.empty()) {
		console::log("[ERROR] No request method found", MSG);
		// 501 Not Implemented
		return false;
	}
	if (method.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ") != std::string::npos) {
		console::log("[ERROR] Invalid method name", MSG);
		// 501 Not Implemented
		return false;
	}
	_request->setMethod(method);
	_current_pos = request_line.find_first_not_of(" ", method_end);
	return true;
}

bool RequestParser::parseUri(std::string request_line) {

	size_t uri_end = request_line.find(" ", _current_pos);
	if (uri_end == std::string::npos) {
		console::log("[ERROR] No SP found after URI", MSG);
		// 404 Not Found
		return false;
	}
	std::string raw_uri = request_line.substr(_current_pos, uri_end - _current_pos);
	raw_uri = trim_whitespaces(raw_uri);
	if (raw_uri.empty()) 
		raw_uri = "/";
	if (raw_uri.length() > MAX_URI_LENGTH) {
		console::log("[ERROR] Request URI too long", MSG);
		// 414 Request-URI Too Long
		return false;
	}
	RequestUri uri(raw_uri);
	if (!uri.parse())
		return false;
	_request->setUri(uri);
	_current_pos = request_line.find_first_not_of(" ", uri_end);
	return true;
}

bool RequestParser::parseVersion(std::string request_line) {

	size_t version = request_line.find("HTTP/", _current_pos);	// could be https
	if (version != std::string::npos) {
		_current_pos += 5;
		size_t dot = request_line.find_first_of('.', _current_pos);
		std::string major = request_line.substr(_current_pos, dot - _current_pos);
		std::string minor = request_line.substr(dot + 1, request_line.size() - (dot + 1));
		_request->setHttpVersion(major, minor);
		return true;
	}
	console::log("Invalid HTTP version in request", ERROR);
	// 505 HTTP Version Not Supported
	return false;
}
 
bool RequestParser::parseHeaders() {

	std::map<std::string, std::vector<std::string> > headers;
	
	if (_current_pos < _raw_request.length())
		_request->setHeadersSize(_raw_request.substr(_current_pos).size());

	while (_current_pos < _raw_request.length()) {
		size_t line_end = _raw_request.find("\r\n", _current_pos);
		if (line_end == std::string::npos) {
			console::log("No CRLF found in headers", ERROR);
			return false;
		}
		std::string header_line = _raw_request.substr(_current_pos, line_end - _current_pos);
		_current_pos = line_end + 2;
		if (header_line.empty())
			break;
		size_t colon_pos = header_line.find(':');
		if (colon_pos == std::string::npos) {
			console::log("Missing colon in header line: ", ERROR);
			return false;
		}
		std::string name = trim_lws(header_line.substr(0, colon_pos));
		if (name.empty()) {
			console::log("Empty header name", MSG);
			return false;
		}
		std::string value = trim_lws(header_line.substr(colon_pos + 1));
		std::vector<std::string> values;
		if (value.find(',') != std::string::npos)
			values = str_to_vect_exept_between(value, ",", "(", ")");	// Comments allowed in User-Agent/Server/Via fields only
		else
			values.push_back(trim_lws(value));
		if (values.empty()) {
			console::log("Empty header value", ERROR);
			return false;
		}
		_request->addHeader(name, values);
	}
	return true;
}

// For HTTP requests, body parsing depends on Content-Length or Transfer-Encoding
bool RequestParser::parseBody() {

	if (_current_pos < _raw_request.length()) {
		std::string body = _raw_request.substr(_current_pos);
		_request->setBody(body);
		_request->setBodySize(_request->getBody().size());
	}
	else {
		_request->setBody("");
		_request->setBodySize(0);
	}
	std::vector<std::string> body_size_value = _request->getHeaderValues("content-length");
	if (!body_size_value.empty()) {
		if (body_size_value.at(0) == "") {
			console::log("Missing \"Content-Length\" header value", MSG);
			return false;
		}
		size_t body_size = to_size_t(body_size_value.at(0));
		if (body_size != _request->getBodySize()) {
			console::log("\"Content-Length\" header value doesn't match body size", MSG);
			return false;
		}
	}	
	return true;
}
