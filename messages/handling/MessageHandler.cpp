#include "MessageHandler.hpp"
#include "../../server/ConnectionManager.hpp"

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

eventProcessor* g_eventProcessor = NULL;
int g_clientFd = -1;

// std::string	handle_messages(const WebservConfig& config, const std::string &raw_request, int port) {
std::string handle_messages(const WebservConfig& config, const std::string &raw_request, int port, eventProcessor* evProc, int clientFd){


	g_eventProcessor = evProc;
	g_clientFd = clientFd;
	
	console::log("========================================", MSG);
	if (port > 0)
		console::log("[INFO] Processing request for port: " + nb_to_string(port), MSG);
	else
		console::log("[INFO] Processing request (default server)", MSG);

	HttpRequest			request;
	MessageHandler		handler(config, &request);

	if (port > 0) {
		if (!config.getServerByPort(port)) {
			console::log("[ERROR][HANDLE MESSAGES] No server config found for port: " + nb_to_string(port), MSG);
			handler.setLastStatus(E_NOT_FOUND);
			handler.generateResponse();
			return handler.serializeResponse();
		}
	}

	if (raw_request.empty()) {
		console::log("[ERROR][HANDLE MESSAGES] Empty request", MSG);
		handler.setLastStatus(E_BAD_REQUEST);
	}
	else if (handler.parseRequest(raw_request, port)) {
		if (handler.validateRequest())
			handler.processRequest();
	}
	handler.generateResponse();
	std::string response = handler.serializeResponse();
	if (response.empty() || (response.find("\r\n\r\n") != std::string::npos && response.find("\r\n\r\n") + 4 >= response.length())){
		//if response empty = is cgi
		return "";
	}
	return response;
}

bool	MessageHandler::parseRequest(const std::string& raw_request, const int& port) {

	RequestParser	parser(_config, _request, raw_request, port);

	if (parser.parseRequest()) {
		parser.setRequestContext();
		_last_status = parser.getLastStatus();
		return true;
	}
	_last_status = parser.getLastStatus();
	return false;
}

bool	MessageHandler::validateRequest() {

	RequestValidator	validator(_config, _request);

	if (validator.validateRequest()) {
		_last_status = validator.getLastStatus();
		return true;
	}
	_last_status = validator.getLastStatus();
	return false;
}

void MessageHandler::processRequest() {

	if (_request->getMethod() == "GET")
		return ;

	RequestProcessor	processor(_config, _request);

	if (_request->getMethod() == "POST")
		processor.processPostRequest();
	else
		processor.processDeleteRequest();
	_last_status = processor.getLastStatus();
}

void MessageHandler::generateResponse() {

	ResponseGenerator	generator(_config, _request, &_response, _last_status);

	generator.generateResponse();
	_last_status = generator.getLastStatus();
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
