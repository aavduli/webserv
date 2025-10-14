#include "MessageHandler.hpp"

MessageHandler::MessageHandler(const WebservConfig& config, HttpRequest* request) : _config(config), _request(request), _response(), _last_status(E_INIT) {}
MessageHandler::MessageHandler(const MessageHandler& rhs) : _config(rhs._config), _request(rhs._request), _response(rhs._response), _last_status(rhs._last_status) {}
MessageHandler& MessageHandler::operator=(const MessageHandler& rhs) {
	if (this != &rhs) {
		_request = rhs._request;
		_response = rhs._response;
		_last_status = rhs._last_status;
	}
	return *this;
}
MessageHandler::~MessageHandler() {}

Status		MessageHandler::getLastStatus() const {return _last_status;}
void		MessageHandler::setLastStatus(Status status) {_last_status = status;}

std::string	handle_messages(const WebservConfig& config, const std::string &raw_request) {

	console::log("=============[NEW REQUEST]=============", MSG);

	HttpRequest			request;
	MessageHandler		handler(config, &request);

	if (raw_request.empty()) {
		console::log("[ERROR][HANDLE MESSAGES] Empty request", MSG);
		handler.setLastStatus(E_BAD_REQUEST);
	}
	else if (handler.parseRequest(raw_request))
		handler.validateRequest();
	handler.generateResponse();

	std::string complete_response = handler.serializeResponse();
	console::log("[DEBUG] Response length: " + nb_to_string(complete_response.length()), MSG);

	return complete_response;
}

bool	MessageHandler::parseRequest(const std::string& raw_request) {

	RequestParser	parser(_config, _request, raw_request);

	if (!parser.parseRequest()) {
		_last_status = parser.getLastStatus();
		return false;
	}
	parser.setRequestContext();
	_last_status = E_OK;
	return true;
}

void MessageHandler::validateRequest() {

	RequestValidator	validator(_config, _request);

	if (!validator.validateRequest())
		_last_status = validator.getLastStatus();
	_last_status = E_OK;
}

bool MessageHandler::generateResponse() {

	ResponseGenerator	generator(_config, _request, &_response);

	generator.setLastStatus(_last_status);
	generator.generateResponse();
	_last_status = generator.getLastStatus();
	return true;
}

std::string	MessageHandler::serializeResponse() {

	int status_code = _response.getStatus();
	const std::string& reason_phrase = status_msg(_response.getStatus());
	const std::string& body = _response.getBody();

	std::ostringstream oss;
	oss << "HTTP/1.0 " << status_code << " " << reason_phrase << "\r\n";
	oss << _response.serializeHeaders();
	oss << "\r\n";
	oss << body;
	return oss.str();
}
