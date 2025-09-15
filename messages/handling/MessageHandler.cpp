#include "MessageHandler.hpp"

MessageHandler::MessageHandler(const HttpRequest* request) : _request(request), _response(NULL) {
	console::log("[MessageHandler Default Constructor]", DEBUG);
}

MessageHandler::MessageHandler(const MessageHandler& rhs) {
	console::log("[MessageHandler Copy Constructor]", DEBUG);
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
	console::log("[MessageHandler Assignement Operator]", DEBUG);
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
	console::log("[MessageHandler Destructor]", DEBUG);
	if (_request)
		delete _request;
	if (_response)
		delete _response;
}

const HttpRequest*	MessageHandler::getRequest() const {
	return _request;
}

HttpResponse*	MessageHandler::getResponse() const {
	return _response;
}

bool	MessageHandler::is_valid_request() const {
	// check against server config
	return true;
}

void	MessageHandler::process_request() {
	return ;
}

// with default headers
void	MessageHandler::generate_response() {
	return ;
}

std::string	MessageHandler::serialize_response() {
	return "";
}