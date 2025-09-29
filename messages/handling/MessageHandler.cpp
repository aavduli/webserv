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
		console::log("[ERROR] Empty request", MSG);
		return ;
	}

	// const char*		resp;
	RequestParser	parser;
	HttpRequest*	request = parser.parse_request(raw);

	if (parser.getState() == s_req_parsing_done) {
		console::log("[INFO] Request parsing success", MSG);
		
		MessageHandler handler(request);
		if (handler.is_valid_request(config)) {
			console::log("[INFO] Request is valid", MSG);
			handler.process_request();
			handler.generate_response();
			// resp = (handler.serialize_response()).c_str();
		}
		else
			console::log("[ERROR] Invalid request in handle_request, state " + parser.getState(), MSG);
	}
	else
		console::log("[ERROR] Request parsing failed with state " + parser.getState(), MSG);
}

// Host header format: "hostname:port" or just "hostname"
bool	is_valid_host(RequestUri *uri, const std::vector<std::string>& header_host, const std::string& config_host) {

	if (uri->getHost().empty() && !header_host.empty()) {
		std::string tmp_host = header_host.at(0);
		size_t colon = tmp_host.find(":");
		if (colon != std::string::npos)
			uri->setHost(tmp_host.substr(0, colon));
		else
			uri->setHost(tmp_host);
	}
	if (!(uri->getHost()).empty() && (uri->getHost()).compare(config_host)) {
		console::log("[ERROR] Invalid host: " + (uri->getHost()), MSG);
		console::log("Expected server host: " + config_host, MSG);
		return false;
	}
	else if ((uri->getHost()).empty())
		uri->setHost(config_host);
	return true;
}

// Host header format: "hostname:port"
bool	is_valid_port(RequestUri *uri, const std::vector<std::string>& header_port, const std::string& config_port) {

	if (uri->getPort().empty() && !header_port.empty()) {
		std::string tmp_port = header_port.at(0);
		size_t colon = tmp_port.find(":");
		if (colon != std::string::npos)
			uri->setPort(tmp_port.substr(colon + 1));
	}
	// Allow default HTTP port (80) or configured port
	if (!(uri->getPort()).empty() && (uri->getPort()) != "80" && (uri->getPort()).compare(config_port)) {
		console::log("[ERROR] Invalid port: " + (uri->getPort()), MSG);
		console::log("Expected server port: " + config_port, MSG);
		return false;
	}
	else if ((uri->getPort()).empty())
		uri->setPort(config_port);
	return true;
}

bool	is_allowed_method(const std::string& method, std::map<std::string, std::string> loc_config) {

	if (loc_config.empty()) {
		console::log("[INFO] No location config found", MSG);
		if (method == "GET" || method == "POST" || method == "DELETE")
			return true;
	}
	else {
		std::string allowed_methods = loc_config["methods"];
		std::vector<std::string> methods = str_to_vect(allowed_methods, " ");
		std::vector<std::string>::iterator it;
		for (it = methods.begin(); it != methods.end(); it++) {
			if (method == *it)
				return true;
		}
	}
	console::log("[ERROR] Invalid method " + method, MSG);
	// Should return 405 Method Not Allowed
	return false;
}

bool	MessageHandler::is_valid_request(const WebservConfig& config) {

	RequestUri uri = _request->getUri();
	// uri.print();
	if (!is_valid_host(&uri, _request->getHeaderValues("host"), config.getDirective("server_name")))
		return false;

	if (!is_valid_port(&uri, _request->getHeaderValues("port"), config.getDirective("port")))
		return false;

	if (!is_allowed_method(_request->getMethod(), config.getLocationConfig(uri.getPath())))
		return false;

	std::string max_body_size = config.getDirective("client_max_body_size");
	if (!max_body_size.empty()) {
		size_t max = to_size_t(max_body_size);
		if (_request->getContentLength() > max) {
			console::log("[ERROR] Content-Length value > client_max_body_size", MSG);
			_state = s_req_invalid_content_length;
			return false;
		}
	}

	// TODO: Handle case when no specific location matches
	// Should validate against default server config and root location "/"
	// This might indicate misconfiguration or need for default handling


	// TODO: Validate path access permissions for this location
	// Check if path is accessible based on location configuration
	
	// TODO: Check location-specific client_max_body_size override
	// Location config takes precedence over server config
	
	// TODO: Validate CGI extension if path targets a script
	// Check location["cgi_ext"] against file extension
	
	// TODO: Handle redirections if location has "return" directive
	// Format: "301 https://example.com" or "302 /other-path"

	// 4. CONTENT-LENGTH VALIDATION
	// Validate request body size against configured maximum
	
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

void	MessageHandler::process_request() {
	
	if (!_request->getMethod().compare("GET")) {
		console::log("[INFO] GET method", MSG);
		handle_get();
	}
	else if (!_request->getMethod().compare("POST")) {
		console::log("[INFO] POST method", MSG);
		handle_post();
	}
	else if (!_request->getMethod().compare("DELETE")) {
		console::log("[INFO] DELETE method", MSG);
		handle_delete();
	}
	else
		console::log("[ERROR] Unkown method", MSG);
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
		console::log("[ERROR] GET request shouldn't have a body", MSG);
		_state = s_req_invalid_get;
		return ;
	}
	// if here, URI should not be empty
	std::string uri = _request->getUri().getRawUri();
}

void	MessageHandler::handle_post() {}

void	MessageHandler::handle_delete() {}

void	MessageHandler::handle_head() {}
