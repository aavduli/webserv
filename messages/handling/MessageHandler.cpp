#include "MessageHandler.hpp"
#include "MessageValidator.hpp"

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
		delete _request;
		delete _response;
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

void	handle_request(const WebservConfig& config, const std::string &raw) {	// TODO should return status enum 

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
			console::log("[ERROR] Invalid request in handle_request", MSG);
	}
	else
		console::log("[ERROR] Request parsing failed with state", MSG);
}

bool	MessageHandler::is_valid_request(const WebservConfig& config) {

	// TODO: Handle case when no specific location matches
	// Should validate against default server config and root location "/"
	// This might indicate misconfiguration or need for default handling

	RequestUri uri = _request->getUri();
	if (!is_valid_host(&uri, _request->getHeaderValues("host"), config.getDirective("server_name"))) 
		return false;
		// 400 Bad Request

	if (!is_valid_port(&uri, _request->getHeaderValues("port"), config.getDirective("port")))
		return false;
		// 400 Bad Request

	if (!is_allowed_method(_request->getMethod(), config.getLocationConfig(uri.getPath())))
		return false;
		// 405 Method Not Allowed
	
	if (!is_valid_version(*_request))
		return false;
		// 505 HTTP Version Not Supported

	if (!is_valid_body_size(_request->getContentLength(), config.getDirective("client_max_body_size")))
		return false;

	if (!is_valid_path(&uri, config, config.getLocationConfig(uri.getPath())))
		return false;

	// TODO: Check location-specific client_max_body_size override
	// Location config takes precedence over server config
	
	// TODO: Validate CGI extension if path targets a script
	// Check location["cgi_ext"] against file extension
	
	// TODO: Handle redirections if location has "return" directive
	// Format: "301 https://example.com" or "302 /other-path"
	
	// 7. REQUIRED HEADERS VALIDATION
	// HTTP/1.1 requires Host header - validate it exists and is valid
	// Check other required headers based on method (Content-Length for POST)
	
	// The Request-URI is transmitted as an encoded string, where some
	// characters may be escaped using the "% HEX HEX" encoding defined by
	// RFC 1738 [4]. The origin server must decode the Request-URI in order
	// to properly interpret the request.
	
	// 10. TRANSFER-ENCODING VALIDATION
	// If Transfer-Encoding header present, validate supported encodings
	// Handle chunked encoding properly
	
	// 11. CONTENT-TYPE VALIDATION
	// For POST/PUT requests, validate Content-Type header presence
	// Check if Content-Type is supported for the target location

	_request->setUri(uri);
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

	/*
	7. RESPONSE GENERATION:
	if (locationConfig["return"])           → Redirect response
	else if (isCGI(extension))             → CGI execution
	else if (isDirectory() && autoindex)   → Directory listing
	else                                   → Static file serving */

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
	// std::string path = _request->getUri().getFullPath();
}

// POST needs Content-Length or Transfer-Encoding header
void	MessageHandler::handle_post() {

	if (!_request->hasHeader("content-length") || _request->getHeaderValues("content-length").empty()) {
		if (_request->getContentLength() == 0)
			return ;
			// 400 Bad Request if it cannot determine the length of the message
		if (!_request->hasHeader("transfer-encoding") || _request->getHeaderValues("transfer-encoding").empty())
			return ;
			// 411 Length Required if it wishes to insist on receiving a valid Content-Length
	}
}

void	MessageHandler::handle_delete() {}
