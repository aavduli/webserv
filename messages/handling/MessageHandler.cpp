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
const HttpResponse	MessageHandler::getResponse() const {return _response;};

// handleMessage() → parseRequest() → validateRequest() → processRequest() → generateResponse()
// TODO should give a way for server to get Status
void	handle_messages(const WebservConfig& config, const std::string &raw_request) {

	console::log("=============[NEW REQUEST]=============", MSG);

	if (raw_request.empty()) {
		console::log("[ERROR] Empty request", MSG);		// return status code? return error/bool?
		return ;
	}

	HttpRequest		request;
	MessageHandler	handler(config, &request);
	
	if (!handler.parseRequest(raw_request)) {
		console::log("[ERROR] Request parsing failed: " + status_msg(handler.getLastStatus()), MSG);
		return ;
	}
	handler.setRequestContext();
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

	RequestParser	parser(_request, raw_request);

	if (!parser.parseRequest()) {
		_last_status = parser.getLastStatus();
		return false;
	}
	_last_status = E_OK;
	return true;
}

void	MessageHandler::setRequestContext() {

	RequestContext ctx;
	
	ctx.location_name = getLocationName();
	if (ctx.location_name.empty())
		ctx.location_config = _config.getServer();
	else
		ctx.location_config = findLocationMatch();

	std::string root = ctx.location_config["root"];
	if (root.empty())
		root = _config.getRoot();
	else
		ctx.document_root = root;

	std::string autoindex = ctx.location_config["autoindex"];
	if (autoindex == "on")
		ctx.autoindex_enabled = true;
	else 
		ctx.autoindex_enabled = false;

	_request->ctx = ctx;
}

bool MessageHandler::validateRequest() {

	MessageValidator	validator(_config, _request);

	if (!validator.validateRequest()) {
		_last_status = validator.getLastStatus();
		return false;
	}
	_last_status = E_OK;
	return true;
}

bool MessageHandler::processRequest() {

	RequestProcessor	processor(_config, *_request);

	processor.processRequest();
	_last_status = processor.getLastStatus();
	return true;
}

bool MessageHandler::generateResponse() {

	ResponseGenerator	generator(_config, _request, &_response);

	generator.generateResponse();
	_last_status = generator.getLastStatus();
	return true;
}

std::string	MessageHandler::getLocationName() {
	
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
