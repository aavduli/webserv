#include "MessageHandler.hpp"

MessageHandler::MessageHandler(HttpRequest* request) : _request(request), _response(NULL) {}

MessageHandler::MessageHandler(const MessageHandler& rhs) {
	if (rhs._request)
		_request = new HttpRequest(*rhs._request);
	else
		_request = NULL;
	if (rhs._response)
		_response = new HttpResponse(*rhs._response);
	else
		_response = NULL;
}

MessageHandler& MessageHandler::operator=(const MessageHandler& rhs) {
	if (this != &rhs) {
		if (rhs._request)
			_request = new HttpRequest(*rhs._request);
		else
			_request = NULL;
		if (rhs._response)
			_response = new HttpResponse(*rhs._response);
		else
			_response = NULL;
	}
	return *this;
}

MessageHandler::~MessageHandler() {
	if (_request)
		delete _request;
	if (_response)
		delete _response;
}

HttpRequest*	MessageHandler::getRequest() const {
	return _request;
}

HttpResponse*	MessageHandler::getResponse() const {
	return _response;
}

void	handle_request(const WebservConfig& config, const std::string &raw) {

	if (raw.empty()) {
		// return status code? return error/bool?
		console::log("Empty request", MSG);
		return ;
	}

	// const char*		resp;
	RequestParser	parser;
	HttpRequest* request = parser.parse_request(raw);

	if (parser.getState() == s_req_parsing_done) {
		console::log("Request parsing success", MSG);
		
		MessageHandler handler(request);
		if (handler.is_valid_request(config)) {
			handler.process_request();
			handler.generate_response();
		}
		// resp = (handler.serialize_response()).c_str();
	}
	else
		std::cout << "[DEBUG] Request parsing failed with state " << parser.getState() << std::endl;
}



bool	is_valid_host(std::string *host, const std::vector<std::string>& header_host, const std::string& config_host) {

	// Host header format: "hostname:port" or just "hostname"
	if ((*host).empty() && !header_host.empty()) {
		std::string tmp_host = header_host.at(0);
		size_t colon = tmp_host.find(":");
		if (colon != std::string::npos)
			*host = tmp_host.substr(0, colon);
		else
			*host = tmp_host;
	}
	
	if (!(*host).empty() && (*host).compare(config_host)) {
		console::log("Invalid host: " + (*host), MSG);
		console::log("Expected server host: " + config_host, MSG);
		return false;
	}
	else if ((*host).empty())
		*host = config_host;
	return true;
}

bool	is_valid_port(std::string *port, const std::vector<std::string>& header_port, const std::string& config_port) {

	// Host header format: "hostname:port"
	if ((*port).empty() && !header_port.empty()) {
		std::string tmp_port = header_port.at(0);
		size_t colon = tmp_port.find(":");
		if (colon != std::string::npos)
			*port = tmp_port.substr(colon + 1);
		// If no colon found, port remains empty (default port assumed)
	}
	
	// Allow default HTTP port (80) or configured port
	if (!(*port).empty() && (*port) != "80" && (*port).compare(config_port)) {
		console::log("[is_valid_port] Invalid port: " + (*port), MSG);
		console::log("[is_valid_port] Expected server port: " + config_port, MSG);
		return false;
	}
	else if ((*port).empty())
		*port = config_port;
	return true;
}

bool	MessageHandler::is_valid_request(const WebservConfig& config) {

	if (!is_valid_host(&_request->getUri().getHost(), _request->getHeaderValues("host"), config.getDirective("host")))
		return false;

	if (!is_valid_port(&_request->getUri().getPort(), _request->getHeaderValues("host"), config.getDirective("listen")))
		return false;

	// 3. PATH AND LOCATION VALIDATION
	std::string	path = _request->getUri().getPath();
	if (!path.empty()) {
		std::map<std::string, std::string> location;
		location = config.getLocationConfig(path);
		if (!location.empty()) {
			// TODO: Validate HTTP method against location's allowed methods
			// Example: location["methods"] contains "GET POST PUT DELETE"
			// Check if _request->getMethod() is in allowed methods list
			
			// TODO: Validate path access permissions for this location
			// Check if path is accessible based on location configuration
			
			// TODO: Check location-specific client_max_body_size override
			// Location config takes precedence over server config
			
			// TODO: Validate CGI extension if path targets a script
			// Check location["cgi_ext"] against file extension
			
			// TODO: Handle redirections if location has "return" directive
			// Format: "301 https://example.com" or "302 /other-path"
		}
		else {
			// TODO: Handle case when no specific location matches
			// Should validate against default server config and root location "/"
			// This might indicate misconfiguration or need for default handling
			console::log("[is_valid_request] No location config found for path: " + path, MSG);
		}
	}
	else {
		// TODO: Handle empty path case - should default to "/" root path
		// Empty path is technically invalid, should set to "/" and retry validation
		console::log("[is_valid_request] Empty path in URI, defaulting to /", MSG);
		// _request->getUri().setPath("/");  // Set default path
		// Return to path validation with default path
	}

	// 4. CONTENT-LENGTH VALIDATION
	// Validate request body size against configured maximum
	std::string max_body_size = config.getDirective("client_max_body_size");
	if (!max_body_size.empty()) {
		size_t max = to_size_t(max_body_size);
		if (_request->getContentLength() > max) {
			console::log("[is_valid_request] Content-Length value > client_max_body_size", ERROR);
			_state = s_req_invalid_content_length;
			return false;
		}
	}

	// TODO: MISSING VALIDATION CHECKS:
	
	// 5. HTTP METHOD VALIDATION
	// Check if the HTTP method is allowed for the matched location
	// Should return 405 Method Not Allowed if method not in location["methods"]
	
	// 6. HTTP VERSION VALIDATION
	// Ensure HTTP version is supported (1.0, 1.1)
	// Check _request->getHttpVersion() against supported versions
	
	// 7. REQUIRED HEADERS VALIDATION
	// HTTP/1.1 requires Host header - validate it exists and is valid
	// Check other required headers based on method (Content-Length for POST)
	
	// 8. URI LENGTH VALIDATION
	// Check if URI length exceeds MAX_URI_LENGTH configuration
	
	// 9. PATH SECURITY VALIDATION
	// Check for directory traversal attacks (../, ..\, etc.)
	// Validate path doesn't contain malicious characters
	
	// 10. TRANSFER-ENCODING VALIDATION
	// If Transfer-Encoding header present, validate supported encodings
	// Handle chunked encoding properly
	
	// 11. CONTENT-TYPE VALIDATION
	// For POST/PUT requests, validate Content-Type header presence
	// Check if Content-Type is supported for the target location

	return true;
}

/*
TODO: validate URI against config
TODO: URL decode special characters (%20, etc.)
TODO: Validate against malicious paths (../, etc.)
*/

void	MessageHandler::process_request() {
	
	if (!_request->getMethod().compare("GET")) {
		console::log("GET method", MSG);
		handle_get();
	}
	else if (!_request->getMethod().compare("POST")) {
		console::log("POST method", MSG);
		handle_post();
	}
	else if (!_request->getMethod().compare("DELETE")) {
		console::log("DELETE method", MSG);
		handle_delete();
	}
	else
		console::log("Unkown method", MSG);
}

// with default headers
void	MessageHandler::generate_response() {
	return ;
}

std::string	MessageHandler::serialize_response() {
	return "coucou";
}

// TODO set response status code and clean exit
void	MessageHandler::handle_get() {

	if (!(_request->getBody().empty())) {
		console::log("GET request shouldn't have a body", ERROR);
		_state = s_req_invalid_get;
		return ;
	}
	// if here, URI should not be empty
	std::string uri = _request->getUri().getRawUri();
}

void	MessageHandler::handle_post() {}

void	MessageHandler::handle_delete() {}

void	MessageHandler::handle_head() {}
