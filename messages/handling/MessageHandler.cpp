#include "MessageHandler.hpp"

MessageHandler::MessageHandler(const WebservConfig& config) : _config(config), _request(), _response(), _last_status(E_INIT) {}
MessageHandler::MessageHandler(const MessageHandler& rhs) : _config(rhs._config), _request(rhs._request), _response(rhs._response), _last_status(rhs._last_status) {}
MessageHandler& MessageHandler::operator=(const MessageHandler& rhs) {
	if (this != &rhs) {
		_last_status = rhs._last_status;
		_request = rhs._request;
		_response = rhs._response;
	}
	return *this;
}
MessageHandler::~MessageHandler() {}

Status	MessageHandler::getLastStatus() const {return _last_status;}

const HttpRequest	MessageHandler::getRequest() const {return _request;};
const HttpResponse	MessageHandler::getResponse() const {return _response;};



// handleMessage() → parseRequest() → validateRequest() → processRequest() → generateResponse()
// TODO should give a way for server to get Status
void	handle_messages(const WebservConfig& config, const std::string &raw_request) {

	console::log("=============[NEW REQUEST]=============", MSG);

	if (raw_request.empty()) {
		console::log("[ERROR] Empty request", MSG);		// return status code? return error/bool?
		return ;
	}

	MessageHandler	handler(config);

	if (!handler.parseRequest(raw_request)) {
		console::log("[ERROR] Request parsing failed: " + status_msg(handler.getLastStatus()), MSG);
		return ;
	}
	if (!handler.validateRequest()) {
		console::log("[ERROR] Request validation failed: " + status_msg(handler.getLastStatus()), MSG);
		return ;
	}
	if (!handler.processRequest()) {
		console::log("[ERROR] Request process failed: " + status_msg(handler.getLastStatus()), MSG);
		return ;
	}
	if (!handler.generateResponse()) {
		console::log("[ERROR] Response generation failed: " + status_msg(handler.getLastStatus()), MSG);
		return ;
	}
	// do something with response
}



bool	MessageHandler::parseRequest(const std::string& raw_request) {

	RequestParser	parser(&_request, raw_request);

	if (!parser.parseRequest()) {
		_last_status = parser.getLastStatus();
		return false;
	}
	_last_status = E_OK;
	return true;
}

bool MessageHandler::validateRequest() {

	MessageValidator	validator(_config, &_request);

	if (!validator.validateRequest()) {
		_last_status = validator.getLastStatus();
		return false;
	}
	_last_status = E_OK;
	_validator = &validator;
	return true;
}

bool MessageHandler::processRequest() {

	RequestProcessor	processor(_config, _request);

	processor.processRequest(_validator);
	_last_status = processor.getLastStatus();
	return true;
}

bool MessageHandler::generateResponse() {

	ResponseGenerator	generator(_config, &_request, &_response);

	generator.generateResponse();
	_last_status = generator.getLastStatus();
	return true;
}