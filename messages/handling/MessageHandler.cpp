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

HttpRequest*	MessageHandler::getRequest() const {return _request;}
HttpResponse*	MessageHandler::getResponse() const {return _response;}
Status			MessageHandler::getStatus() const {return _status;}

void	handle_request(const WebservConfig& config, const std::string &raw) {	// TODO should return status enum 

	console::log("=============[NEW REQUEST]=============", MSG);
	if (raw.empty()) {
		// return status code? return error/bool?
		console::log("[ERROR] Empty request", MSG);
		return ;
	}

	HttpRequest request;
	RequestParser parser;
	
	if (!parser.parseRequest(&request, raw)) {
		console::log("[ERROR] Request parsing failed", MSG);
		return;
	}
	MessageValidator validator(config, request);

	if (validator.isValidRequest()) {
		console::log("[INFO] Request is valid", MSG);
		MessageHandler handler(&request);  // Pass pointer to stack object

		handler.processRequest();
		handler.generateResponse();
		// const char* resp = (handler.serializeResponse()).c_str();
	}
	else {
		console::log("[ERROR] Invalid request: " + status_msg(validator.getLastStatus()), MSG);
		// TODO: Generate error response based on validator.getLastStatus()
	}
}

void	MessageHandler::processRequest() {
	
	if (!_request->getMethod().compare("GET")) {
		console::log("[INFO] GET method", MSG);
		handleGet();
	}
	else if (!_request->getMethod().compare("POST")) {
		console::log("[INFO] POST method", MSG);
		handlePost();
	}
	else if (!_request->getMethod().compare("DELETE")) {
		console::log("[INFO] DELETE method", MSG);
		handleDelete();
	}
	else
		console::log("[ERROR] Unkown method", MSG);
}

// with default headers
void	MessageHandler::generateResponse() {

	/*
	7. RESPONSE GENERATION:
	if (locationConfig["return"])           → Redirect response
	else if (isCGI(extension))             → CGI execution
	else if (isDirectory() && autoindex)   → Directory listing
	else                                   → Static file serving */

	/* 
	HTTP/1.1 Changes:
	Persistent connections are DEFAULT (keep-alive)
	Only close if client sends Connection: close or error occurs
	Must handle pipelined requests
	
	// HTTP/1.1 response format changes
	void generateResponse(HttpResponse* response, HttpRequest* request) {
		// Always use HTTP/1.1 in responses (backward compatible)
		response->setHttpVersion("1", "1");
		
		// Required headers
		response->addHeader("Date", getCurrentHTTPDate());
		
		// Handle Connection
		bool close_connection = shouldCloseConnection(request);
		response->addHeader("Connection", close_connection ? "close" : "keep-alive");
		
		// Content-Length or Transfer-Encoding required
		if (has_body && !using_chunked) {
			response->addHeader("Content-Length", toString(body_length));
		}
	}
	
	// date header obligatory

	*/

	return ;
}

std::string	MessageHandler::serializeResponse() {
	return "coucou";
}

// TODO set response status code and clean exit
void	MessageHandler::handleGet() {

	if (!(_request->getBody().empty())) {
		console::log("[ERROR] GET request shouldn't have a body", MSG);
		// _state = s_req_invalid_get;
		return ;
	}
	// if here, URI should not be empty
	// std::string path = _request->getUri().getFullPath();
}

// POST needs Content-Length or Transfer-Encoding header
void	MessageHandler::handlePost() {

	if (!_request->hasHeader("content-length") || _request->getHeaderValues("content-length").empty()) {
		if (_request->getBodySize() == 0)
			return ;
			// 400 Bad Request if it cannot determine the length of the message
		if (!_request->hasHeader("transfer-encoding") || _request->getHeaderValues("transfer-encoding").empty())
			return ;
			// 411 Length Required if it wishes to insist on receiving a valid Content-Length
	}

	// bool is_valid_content_type(const std::string& method, 
	// 						const std::vector<std::string>& ct_headers,
	// 						const std::map<std::string, std::string>& location);
	// Validate Content-Type for POST/PUT
	// Check against allowed types in location config
}

void	MessageHandler::handleDelete() {}
