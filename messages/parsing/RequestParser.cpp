#include "MessageParser.hpp"
#include "../data/HttpRequest.hpp"
#include <iostream>
#include <sstream>

RequestParser::RequestParser() : MessageParser(), _request(NULL) {
	console::log("RequestParser Constructor", DEBUG);
}

RequestParser::RequestParser(const RequestParser& rhs) : MessageParser(rhs) {
	console::log("RequestParser copy constructor", DEBUG);
	if (rhs._request)
		_request = new HttpRequest(*rhs._request);
	else
		_request = NULL;
}

RequestParser& RequestParser::operator=(const RequestParser& rhs) {
	console::log("RequestParser assignment operator", DEBUG);
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

RequestParser::~RequestParser() {
	console::log("RequestParser destructor", DEBUG);
	if (_request)
		delete _request;
}

HttpRequest* RequestParser::parse_request(std::string raw_request) {
	console::log("RequestParser parse_request", DEBUG);
	
	// Initialize parsing state
	_state = s_req_start;
	_raw_data = raw_request;
	
	// Create new HttpRequest object
	HttpRequest* request = new HttpRequest();
	_request = request;
	
	// Parse through all states
	while (_state != s_msg_done && _state != s_msg_error) {
		switch (_state) {
			case s_req_start:
				if (!parse_request_line() && _state != s_req_done) {
					console::log("Failed to parse request line", ERROR);
					delete request;
					return NULL;
				}
				_state = s_head_start;
				break;
				
			case s_head_start:
				if (!parse_headers() && _state != s_head_done) {
					console::log("Failed to parse headers", ERROR);
					delete request;
					return NULL;
				}
				_state = s_body_start;
				break;
				
			case s_body_start:
				if (!parse_body() && _state != s_body_done) {
					console::log("Failed to parse body", ERROR);
					delete request;
					return NULL;
				}
				_state = s_msg_done;
				break;
				
			default:
				console::log("Unknown parsing state", ERROR);
				delete request;
				return NULL;
		}
	}
	
	if (_state == s_msg_error) {
		console::log("Parsing failed", ERROR);
		delete request;
		return NULL;
	}
	
	console::log("Parsing completed successfully", DEBUG);
	return request;
}

// Request-Line = Method SP Request-URI SP HTTP-Version CRLF

bool RequestParser::parse_request_line() {
	console::log("RequestParser parse_request_line", DEBUG);
	_state = s_req_line;
	
	// Find end of request line CRLF '\r\n'
	size_t line_end = _raw_data.find("\r\n", _current_pos);
	if (line_end == std::string::npos) {
		console::log("No CRLF found in request line", ERROR);
		return false;
	}
	
	// Extract request line
	std::string request_line = _raw_data.substr(_current_pos, line_end - _current_pos);

	// trim leading and trailing whitespaces
	std::vector<std::string> 	whitespaces;
	whitespaces.push_back(" ");
	whitespaces.push_back("\t");
	request_line = trim(request_line, whitespaces);
	std::cout << "[DEBUG] Request line: " << request_line << std::endl;
	
	// get method, URI, and version from request line
	if (!parse_method(request_line)) {
		console::log("Failed to parse method", ERROR);
		return false;
	}
	if (!parse_uri(request_line)) {
		console::log("Failed to parse uri", ERROR);
		return false;
	}
	if (!parse_version(request_line)) {
		console::log("Failed to parse version", ERROR);
		return false;
	}
	
	_current_pos = line_end + 2;	// move past CRLF
	_state = s_req_done;
	return true;
}

bool RequestParser::parse_method(std::string request_line) {
	console::log("RequestParser parse_method", DEBUG);

	// Find SP between method and URI
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
	_request->setMethod(method);
	_current_pos = method_end + 1;	// move past SP

	std::cout << "[DEBUG] Method parsed - " << method << std::endl;
	return true;
}

bool RequestParser::parse_uri(std::string request_line) {
	console::log("RequestParser parse_uri", DEBUG);

	// Find SP between URI and version
	size_t uri_end = request_line.find(" ", _current_pos);
	if (uri_end == std::string::npos) {
		console::log("No SP found after URI", ERROR);
		return false;
	}
	std::string uri = request_line.substr(_current_pos, uri_end - _current_pos);
	if (uri.empty()) {
		console::log("No request URI found", ERROR);
		return false;
	}
	_request->setUri(uri);
	_current_pos = uri_end + 1;	// move past SP

	std::cout << "[DEBUG] URI parsed - " << uri << std::endl;
	return true;
}

bool RequestParser::parse_version(std::string request_line) {
	console::log("RequestParser parse_version", DEBUG);

	// Last element is version
	std::string version = request_line.substr(_current_pos, request_line.size() - _current_pos);
	if (version.empty()) {
		console::log("No request version found", ERROR);
		return false;
	}
	_request->setHttpVersion(version);
	std::cout << "[DEBUG] Version parsed - " << version << std::endl;
	return true;
}

bool RequestParser::parse_headers() {
	console::log("RequestParser parse_headers", DEBUG);
	_state = s_head_fields;

	std::map<std::string, std::vector<std::string> > headers;
	
	// loop through all headers
	while (_current_pos < _raw_data.length()) {

		// Find the end of current line
		size_t line_end = _raw_data.find("\r\n", _current_pos);
		if (line_end == std::string::npos) {
			console::log("No CRLF found in headers", ERROR);
			return false;
		}
		
		// Extract current line
		std::string line = _raw_data.substr(_current_pos, line_end - _current_pos);
		_current_pos = line_end + 2; // Move past \r\n
		
		// Empty line (containing only CRLF) indicates end of headers
		if (line.empty()) {
			console::log("End of headers found", DEBUG);
			break;
		}
		
		// Parse header: "Name: Value"
		size_t colon_pos = line.find(':');
		if (colon_pos == std::string::npos) {
			console::log("Missing colon in header line: ", ERROR);
			std::cout << line << std::endl;
			return false;
		}
		
		std::string name = line.substr(0, colon_pos);
		std::string value = line.substr(colon_pos + 1);
		std::cout << "[DEBUG] Header parsed - " << name << ": " << value << std::endl;

		// Split if multiple comma-separated strings in value
		// TODO: check for commas in comments () which must not be split
		std::vector<std::string>	values = str_to_vect(value, ",");

		_request->addHeader(name, values);
	}
	
	_state = s_head_done;
	return true;
}

bool RequestParser::parse_body() {
	console::log("RequestParser parse_body", DEBUG);
	
	// For HTTP requests, body parsing depends on Content-Length or Transfer-Encoding
	// For now, read the remaining data as body
	if (_current_pos < _raw_data.length()) {
		std::string body = _raw_data.substr(_current_pos);
		_request->setBody(body);
		std::cout << "[DEBUG] Body parsed, length: " << body.length() << std::endl;
	}
	else
		console::log("No body found", DEBUG);
	return true;
}
