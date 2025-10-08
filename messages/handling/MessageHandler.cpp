#include "MessageHandler.hpp"

MessageHandler::MessageHandler(const WebservConfig& config, HttpRequest* request) : _config(config), _request(request), _response(), _last_status(E_INIT) {}
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

Status				MessageHandler::getLastStatus() const {return _last_status;}
void				MessageHandler::setLastStatus(Status status) {_last_status = status;}

// handleMessage() → parseRequest() → validateRequest() → processRequest() → generateResponse()
// TODO should give a way for server to get Status
std::string	handle_messages(const WebservConfig& config, const std::string &raw_request) {

	console::log("=============[NEW REQUEST]=============", MSG);

	HttpRequest		request;
	std::string		response;
	MessageHandler	handler(config, &request);

	if (raw_request.empty()) {
		console::log("[ERROR] Empty request", MSG);
		handler.setLastStatus(E_BAD_REQUEST);
	}
	else if (handler.parseRequest(raw_request)) {
		// only set context if parsing OK
		handler.setRequestContext();
		handler.validateRequest();
	}
	if (!handler.generateResponse()) {
		console::log("[ERROR] Response generation failed: " + status_msg(handler.getLastStatus()), MSG);
		// critical error
		return response;
	}
	response = handler.serializeResponse();
	return response;
}

bool	MessageHandler::parseRequest(const std::string& raw_request) {

	RequestParser	parser(_request, raw_request);

	if (!parser.parseRequest()) {
		_last_status = parser.getLastStatus();
		return false;
	}
	_last_status = E_OK;
	return true;
}

bool MessageHandler::validateRequest() {
	
	RequestValidator	validator(_config, _request);
	
	if (!validator.validateRequest()) {
		_last_status = validator.getLastStatus();
		return false;
	}
	_last_status = E_OK;
	return true;
}

bool MessageHandler::generateResponse() {
	
	ResponseGenerator	generator(_config, _request, &_response);
	
	generator.setLastStatus(_last_status);
	generator.generateResponse();
	_last_status = generator.getLastStatus();
	return true;
}

std::string MessageHandler::serializeResponse() {

	// if (!_response) {
	// 	console::log("[ERROR] No response to serialize", MSG);
	// 	return "";
	// }
	
	// HTTP/1.1 status line
	// All headers with proper CRLF
	// Empty line
	// Body content

	// TODO: Implement response serialization
	// Format: HTTP/1.1 200 OK\r\nHeaders\r\n\r\nBody
	console::log("[INFO] Serializing response", MSG);
	return "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
}

void	MessageHandler::setRequestContext() {

	RequestContext ctx(_config);
	
	ctx.setLocationName(findConfigLocationName());
	if (ctx.getLocationName().empty())
		ctx.setLocationConfig(_config.getServer());
	else
		ctx.setLocationConfig(findLocationMatch());

	std::map<std::string, std::string> config = ctx.getLocationConfig();
	std::string root = config["root"];
	if (root.empty())
		ctx.setDocumentRoot(_config.getRoot());
	else
		ctx.setDocumentRoot(root);

	std::string index = config["index"];
	std::vector<std::string> indexes = str_to_vect(index, " ");
	ctx.setIndexList(indexes);

	std::string autoindex = config["autoindex"];
	if (autoindex == "on")
		ctx.setAutoindexEnabled(true);
	else
		ctx.setAutoindexEnabled(false);

	_request->ctx = ctx;
}

std::string	MessageHandler::findConfigLocationName() {
	
	std::string path = _request->getUri().getPath();
	if (_config.hasLocation(path))
		return path;
	
	std::string match = "";
	std::string test_path = path;
	while (!test_path.empty()) {
		if (_config.hasLocation(test_path)) {
			if (test_path.length() > match.length()) {
				match = test_path;
			}
		}
		size_t last_slash = test_path.find_last_of('/');
		if (last_slash == 0) {
			test_path = "/";
			if (_config.hasLocation(test_path)) {
				if (match.empty())
					match = test_path;
			}
			break;
		}
		else if (last_slash == std::string::npos)
			break;
		else
			test_path = test_path.substr(0, last_slash);
	}
	return match;
}

// Uses longest prefix matching
std::map<std::string, std::string>	MessageHandler::findLocationMatch() {
	
	std::string path = _request->getUri().getPath();
	if (_config.hasLocation(path))
		return _config.getLocationConfig(path);
	
	std::string match = "";
	std::string test_path = path;
	std::map<std::string, std::string> best_config;
	while (!test_path.empty()) {
		if (_config.hasLocation(test_path)) {
			std::map<std::string, std::string> config = _config.getLocationConfig(test_path);
			if (test_path.length() > match.length()) {
				match = test_path;
				best_config = config;
			}
		}
		size_t last_slash = test_path.find_last_of('/');
		if (last_slash == 0) {
			test_path = "/";
			if (_config.hasLocation(test_path)) {
				std::map<std::string, std::string> config = _config.getLocationConfig(test_path);
				if (match.empty())
					best_config = config;
			}
			break;
		}
		else if (last_slash == std::string::npos)
			break;
		else
			test_path = test_path.substr(0, last_slash);
	}
	return best_config;
}
