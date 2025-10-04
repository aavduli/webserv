#include "MessageHandler.hpp"
#include "MessageValidator.hpp"
#include "RequestHandler.hpp"
#include "ResponseHandler.hpp"

MessageHandler::MessageHandler(HttpRequest* request) : _last_status(E_INIT), _request(request), _response(NULL) {}

MessageHandler::MessageHandler(const MessageHandler& rhs) : _last_status(rhs._last_status), _request(rhs._request) {
	if (rhs._response)
		_response = new HttpResponse(*rhs._response);
	else
		_response = NULL;
}

MessageHandler& MessageHandler::operator=(const MessageHandler& rhs) {
	if (this != &rhs) {
		delete _response;
		_last_status = rhs._last_status;
		_request = rhs._request;
		if (rhs._response)
			_response = new HttpResponse(*rhs._response);
		else
			_response = NULL;
	}
	return *this;
}

MessageHandler::~MessageHandler() {
	if (_response)
		delete _response;
}

Status			MessageHandler::getLastStatus() const {return _last_status;}
void			MessageHandler::setLastStatus(Status status) {_last_status = status;}
HttpRequest*	MessageHandler::getRequest() const {return _request;};
HttpResponse*	MessageHandler::getResponse() const {return _response;};

// TODO should give a way for server to get Status
void	handle_messages(const WebservConfig& config, const std::string &raw_request) {

	console::log("=============[NEW REQUEST]=============", MSG);

	// should never be empty?
	if (raw_request.empty()) {
		// return status code? return error/bool?
		console::log("[ERROR] Empty request", MSG);
		return ;
	}

	HttpRequest request;
	RequestParser requ_parser;
	if (!requ_parser.parseRequest(&request, raw_request)) {
		console::log("[ERROR] Request parsing failed", MSG);
		return;
	}

	MessageValidator validator(config, request);
	if (validator.isValidRequest()) {
		console::log("[INFO] Request is valid", MSG);

		RequestHandler requ_handler(&request);
		requ_handler.setLastStatus(validator.getLastStatus());
		requ_handler.processRequest();
		
		// Use ResponseHandler for generating the response
		ResponseHandler resp_handler(&request, config);
		resp_handler.setLastStatus(requ_handler.getLastStatus());
		resp_handler.generateResponse();
		
		// const char* resp = (resp_handler.serializeResponse()).c_str();
	}
	else {
		console::log("[ERROR] Invalid request: " + status_msg(validator.getLastStatus()), MSG);
		// TODO: Generate error response based on validator.getLastStatus()
	}
}
