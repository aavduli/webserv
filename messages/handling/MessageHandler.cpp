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

Status		MessageHandler::getLastStatus() const {return _last_status;}
void		MessageHandler::setLastStatus(Status status) {_last_status = status;}

// TODO should give a way for server to get Status
std::string	handle_messages(const WebservConfig& config, const std::string &raw_request) {

	console::log("=============[NEW REQUEST]=============", MSG);

	HttpRequest			request;
	MessageHandler		handler(config, &request);

	if (raw_request.empty()) {
		console::log("[ERROR][HANDLE MESSAGES] Empty request", MSG);
		handler.setLastStatus(E_BAD_REQUEST);
	}
	else if (handler.parseRequest(raw_request)) {
		handler.setRequestContext();
		handler.validateRequest();
	}
	handler.generateResponse();
	console::log("[DEBUG] Status after generateResponse: " + status_msg(handler.getLastStatus()), MSG);

	std::string complete_response = handler.serializeResponse();
	console::log("[DEBUG] Response length: " + nb_to_string(complete_response.length()), MSG);

	return complete_response;
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
	
	ctx._location_name = findConfigLocationName();
	if (ctx._location_name.empty())
		ctx._location_config = _config.getServer();
	else
		ctx._location_config = findLocationMatch();

	std::map<std::string, std::string> config = ctx._location_config;
	std::string root = config["root"];
	if (root.empty())
		ctx._document_root = _config.getRoot();
	else
		ctx._document_root = root;

	std::string index = config["index"];
	std::vector<std::string> indexes = str_to_vect(index, " ");
	ctx._index_list = indexes;

	std::string autoindex = config["autoindex"];
	if (autoindex == "on")
		ctx._autoindex_enabled = true;
	else
		ctx._autoindex_enabled = false;

	_request->ctx = ctx;
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
	
	ResponseGenerator	generator(_config, _request, &_response, _last_status);
	
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
	oss << _response.serializeHeaders() << "\r\n";
	oss << "\r\n";
	oss << body;
	return oss.str();
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
