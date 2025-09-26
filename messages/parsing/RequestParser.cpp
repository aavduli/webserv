#include "RequestParser.hpp"
#include "../data/HttpRequest.hpp"

RequestParser::RequestParser() : MessageParser(), _request(NULL) {}

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
					console::log("Failed to parse request line", ERROR);
					_state = s_msg_error;
				}
				_state = s_req_parsing_headers;
				break;
			case s_req_parsing_headers:
				if (!parse_headers()) {
					console::log("Failed to parse headers", ERROR);
					_state = s_msg_error;
				}
				_state = s_req_parsing_body;
				break;
			case s_req_parsing_body:
				if (!parse_body()) {
					console::log("Failed to parse body", ERROR);
					_state = s_msg_error;
				}
				_state = s_req_parsing_done;
				break;
			default:
				console::log("Unkown parsing state", ERROR);
				_state = s_msg_error;
		}
	}
	if (_state == s_msg_error) {
		console::log("Parsing failed", ERROR);
		delete request;
		return NULL;
	}
	std::cout << GREEN;
	print_request(request);
	std::cout << RESET << std::endl;
	return request;
}

bool RequestParser::parse_request_line() {

	size_t line_end = _raw_data.find("\r\n", _current_pos);
	if (line_end == std::string::npos) {
		console::log("No CRLF found in request line", ERROR);
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
		console::log("No SP found after method", ERROR);
		return false;
	}
	std::string method = request_line.substr(_current_pos, method_end - _current_pos);
	if (method.empty()) {
		console::log("No request method found", ERROR);
		return false;
	}
	if (method.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ") != std::string::npos) {
		console::log("Invalid method name", ERROR);
		return false;
	}
	_request->setMethod(method);
	_current_pos = request_line.find_first_not_of(" ", method_end);
	return true;
}

bool RequestParser::parse_uri(std::string request_line) {

	size_t uri_end = request_line.find(" ", _current_pos);
	if (uri_end == std::string::npos) {
		console::log("No SP found after URI", ERROR);
		return false;
	}
	std::string raw_uri = request_line.substr(_current_pos, uri_end - _current_pos);
	raw_uri = trim_whitespaces(raw_uri);
	if (raw_uri.empty()) {
		console::log("Empty request URI", ERROR);
		return false;
	}
	if (raw_uri.length() > MAX_URI_LENGTH) {
		console::log("Request URI too long", ERROR);
		return false;
	}
	RequestUri uri(raw_uri);
	if (!uri.parse())
		return false;
	_request->setUri(uri);
	_current_pos = request_line.find_first_not_of(" ", uri_end);
	return true;
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
	console::log("Invalid HTTP version in request", ERROR);
	return false;
}
 
bool RequestParser::parse_headers() {

	std::map<std::string, std::vector<std::string> > headers;
	
	while (_current_pos < _raw_data.length()) {
		size_t line_end = _raw_data.find("\r\n", _current_pos);
		if (line_end == std::string::npos) {
			console::log("No CRLF found in headers", ERROR);
			return false;
		}
		std::string header_line = _raw_data.substr(_current_pos, line_end - _current_pos);
		_current_pos = line_end + 2;
		if (header_line.empty())
			break;
		size_t colon_pos = header_line.find(':');
		if (colon_pos == std::string::npos) {
			console::log("Missing colon in header line: ", ERROR);
			return NULL;
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
bool RequestParser::parse_body() {

	if (_current_pos < _raw_data.length()) {
		std::string body = _raw_data.substr(_current_pos);
		_request->setBody(body);
		_request->setContentLength(_request->getBody().size());
	}
	else {
		_request->setBody("");
		_request->setContentLength(0);
		console::log("Empty body", MSG);
	}
	std::vector<std::string> content_length_value = _request->getHeaderValues("content-length");
	if (content_length_value.empty())
		console::log("Missing \"Content-Length\" header field", MSG);
	else {
		if (content_length_value.at(0) == "") {
			console::log("Missing \"Content-Length\" header value", MSG);
			_state = s_req_invalid_content_length;
			return false;
		}
		size_t content_length = to_size_t(content_length_value.at(0));
		if (content_length != _request->getContentLength()) {
			console::log("\"Content-Length\" header value doesn't match body size", MSG);
			_state = s_req_invalid_content_length;
			return false;
		}
		// 400 (bad request) if it cannot determine the length of the message OR
		// 411 (length required) if it wishes to insist on receiving a valid Content-Length
		// console::log("Status 400/411", ERROR);
	}
	return true;
}
