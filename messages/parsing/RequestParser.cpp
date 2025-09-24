#include "RequestParser.hpp"
#include "../data/HttpRequest.hpp"

RequestParser::RequestParser(const WebservConfig& config) : MessageParser(config), _request(NULL) {}

RequestParser::RequestParser(const RequestParser& rhs) : MessageParser(rhs) {
	if (rhs._request)
		_request = new HttpRequest(*rhs._request);
	else
		_request = NULL;
}

RequestParser& RequestParser::operator=(const RequestParser& rhs) {
	if (this != &rhs) {
		MessageParser::operator=(rhs);
		delete _request;
		if (rhs._request)
			_request = new HttpRequest(*rhs._request);
		else
			_request = NULL;
	}
	return *this;
}

RequestParser::~RequestParser() {}

HttpRequest* RequestParser::parse_request(std::string raw_request) {

	_state = s_req_parsing_start;
	_raw_data = raw_request;
	HttpRequest* request = new HttpRequest();
	_request = request;
	
	while (_state != s_req_parsing_done && _state != s_msg_error) {
		switch (_state) {
			case s_req_parsing_start:
				if (!parse_request_line()) {
					// console::log("Failed to parse request line", ERROR, ALL);
					_state = s_msg_error;
				}
				_state = s_req_parsing_headers;
				break;
			case s_req_parsing_headers:
				if (!parse_headers()) {
					// console::log("Failed to parse headers", ERROR, ALL);
					_state = s_msg_error;
				}
				_state = s_req_parsing_body;
				break;
			case s_req_parsing_body:
				if (!parse_body()) {
					// console::log("Failed to parse body", ERROR, ALL);
					_state = s_msg_error;
				}
				_state = s_req_parsing_done;
				break;
			default:
				// console::log("Unknown parsing state", ERROR, ALL);
				_state = s_msg_error;
		}
	}
	if (_state == s_msg_error) {
		// console::log("Parsing failed", ERROR, ALL);
		delete request;
		return NULL;
	}
	return request;
}

bool RequestParser::parse_request_line() {

	size_t line_end = _raw_data.find("\r\n", _current_pos);
	if (line_end == std::string::npos) {
		// console::log("No CRLF found in request line", ERROR, ALL);
		return false;
	}
	std::string request_line = _raw_data.substr(_current_pos, line_end - _current_pos);
	request_line = trim_lws(request_line);
	
	if (!parse_method(request_line))
		return false;
	if (!parse_uri(request_line))
		return false;
	if (!parse_version(request_line))
		return false;
	_current_pos = line_end + 2;
	return true;
}

HttpMethod	string_to_method(std::string str) {
	if (str.compare("GET") == 0)
		return GET;
	else if (str.compare("POST") == 0)
		return POST;
	else if (str.compare("DELETE") == 0)
		return DELETE;
	else if (str.compare("HEAD") == 0)
		return HEAD;
	else if (str.compare("PUT") == 0)
		return PUT;
	else if (str.compare("CONNECT") == 0)
		return CONNECT;
	else if (str.compare("OPTIONS") == 0)
		return OPTIONS;
	else if (str.compare("TRACE") == 0)
		return TRACE;
	else
		return UNKNOWN;
}

bool RequestParser::parse_method(std::string request_line) {

	size_t method_end = request_line.find(" ", _current_pos);
	if (method_end == std::string::npos) {
		std::cout << "[AH] No SP found after method" << std::endl;
		// console::log("No SP found after method", ERROR, ALL);
		return false;
	}
	std::string method = request_line.substr(_current_pos, method_end - _current_pos);
	if (method.empty()) {
		std::cout << "[AH] No request method found" << std::endl;
		// console::log("No request method found", ERROR, ALL);
		return false;
	}
	HttpMethod e_method = string_to_method(method);
	if (e_method == UNKNOWN) {
		std::cout << "[AH] Unknown method" << std::endl;
		// console::log("Unknown method", ERROR, ALL);
		return false;
	}
	_request->setMethod(e_method);
	_current_pos = request_line.find_first_not_of(" ", method_end);
	return true;
}

bool RequestParser::parse_uri(std::string request_line) {

	size_t uri_end = request_line.find(" ", _current_pos);
	if (uri_end == std::string::npos) {
		std::cout << "[AH] No SP found after URI" << std::endl;
		// console::log("No SP found after URI", ERROR, ALL);
		return false;
	}
	std::string raw_uri = request_line.substr(_current_pos, uri_end - _current_pos);
	raw_uri = trim_whitespaces(raw_uri);
	if (raw_uri.empty()) {
		std::cout << "[AH] Empty request URI" << std::endl;
		// console::log("Empty request URI", ERROR, ALL);
		return false;
	}
	if (raw_uri.length() > MAX_URI_LENGTH) {
		std::cout << "[AH] Request URI too long" << std::endl;
		// console::log("Request URI too long", ERROR, ALL);
		return false;
	}

	RequestUri uri(raw_uri);
	if (uri.parse()) {
		_request->setUri(uri);
		// uri.print();
		_current_pos = request_line.find_first_not_of(" ", uri_end);
		return true;
	}
	else
		return false;
}

bool RequestParser::parse_version(std::string request_line) {

	size_t version = request_line.find("HTTP/", _current_pos);
	if (version != std::string::npos) {
		_current_pos += 5;
		size_t dot = request_line.find_first_of('.', _current_pos);
		std::string major = request_line.substr(_current_pos, dot - _current_pos);
		std::string minor = request_line.substr(dot + 1, request_line.size() - (dot + 1));
		_request->setHttpVersion(major, minor);
		return true;
	}
	std::cout << "[AH] Invalid HTTP version in request" << std::endl;
	// console::log("Invalid HTTP version in request", ERROR, ALL);
	return false;
}

bool RequestParser::parse_headers() {

	std::map<std::string, std::vector<std::string> > headers;
	
	while (_current_pos < _raw_data.length()) {
		size_t line_end = _raw_data.find("\r\n", _current_pos);
		if (line_end == std::string::npos) {
			std::cout << "[AH] No CRLF found in headers" << std::endl;
			// console::log("No CRLF found in headers", ERROR, ALL);
			return false;
		}
		std::string header_line = _raw_data.substr(_current_pos, line_end - _current_pos);
		_current_pos = line_end + 2;
		if (header_line.empty())
			break;
		size_t colon_pos = header_line.find(':');
		if (colon_pos == std::string::npos) {
			std::cout << "[AH] Missing colon in header line: " << header_line << std::endl;
			// console::log("Missing colon in header line: ", ERROR, ALL);
			return NULL;
		}
		std::string name = trim_lws(header_line.substr(0, colon_pos));
		if (name.empty()) {
			std::cout << "[AH] Empty header name" << std::endl;
			// console::log("Empty header name", INFO, AH);
			return false;
		}
		std::string value = trim_lws(header_line.substr(colon_pos + 1));
		std::vector<std::string> values;
		if (value.find(',') != std::string::npos)
			values = str_to_vect_exept_between(value, ",", "(", ")");	// Comments allowed in User-Agent/Server/Via fields only
		else
			values.push_back(trim_lws(value));
		if (values.empty()) {
			std::cout << "[AH] Empty header value" << std::endl;
			// console::log("Empty header value", INFO, AH);
			return false;
		}
		_request->addHeader(name, values);
	}
	return true;
}

// For HTTP requests, body parsing depends on Content-Length or Transfer-Encoding
// For now, read the remaining data as body
bool RequestParser::parse_body() {
	
	if (_current_pos < _raw_data.length()) {
		std::string body = _raw_data.substr(_current_pos);
		_request->setBody(body);
	}
	else {
		_request->setBody("");
		// std::cout << "[AH] Warning: Empty body" << std::endl;
		// console::log("Empty body", WARNING, ALL);
	}
	return true;
}
