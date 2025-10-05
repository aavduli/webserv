#include "RequestProcessor.hpp"
#include "../../console/console.hpp"

RequestProcessor::RequestProcessor(const WebservConfig& config, const HttpRequest& request) : _config(config), _request(request), _last_status(E_INIT) {}
RequestProcessor::RequestProcessor(const RequestProcessor& rhs) : _config(rhs._config), _request(rhs._request), _last_status(rhs._last_status) {}
RequestProcessor& RequestProcessor::operator=(const RequestProcessor& rhs) {
	if (this != &rhs) {
		_last_status = rhs._last_status;
	}
	return *this;
}
RequestProcessor::~RequestProcessor() {}
Status	RequestProcessor::getLastStatus() const {return _last_status;}

void RequestProcessor::processRequest(MessageValidator* validator) {
	
	const std::string& method = _request.getMethod();
	
	if (method == "GET") {
		console::log("[INFO] Processing GET method", MSG);
		processGetRequest(validator);
	}
	else if (method == "POST" ) {
		console::log("[INFO] Processing POST method", MSG);
		processPostRequest();
	}
	else if (method == "DELETE") {
		console::log("[INFO] Processing DELETE method", MSG);
		processDeleteRequest();
	}
	else {
		console::log("[ERROR] Unknown method: " + method, MSG);
		_last_status = E_METHOD_NOT_ALLOWED;
	}
}

bool RequestProcessor::validateGetRequest(MessageValidator* validator) {

	if (!_request.getBody().empty()) {
		console::log("[ERROR] GET request shouldn't have a body", MSG);
		_last_status = E_BAD_REQUEST;
		return false;
	}
	
	std::string path = _request.getUri().getEffectivePath();
	if (is_directory(path)) {
		std::map<std::string, std::string> loc_conf = validator->getLocationConfig();
		std::string index = loc_conf["index"];
		if (index.empty())
			index = _config.getIndex();
		path = resolve_index_file(path, index);

		if (!is_valid_file_path(path)) {
			_last_status = E_NOT_FOUND;
			return false;
		}
	}
	return true;
}

bool RequestProcessor::validatePostRequest() {
	// POST needs Content-Length or Transfer-Encoding header
	if (!_request.hasHeader("content-length") || _request.getHeaderValues("content-length").empty()) {
		if (_request.getBodySize() == 0) {
			// Empty body is allowed for POST
			return true;
		}
		// 400 Bad Request if it cannot determine the length of the message
		if (!_request.hasHeader("transfer-encoding") || _request.getHeaderValues("transfer-encoding").empty()) {
			console::log("[ERROR] POST request missing Content-Length or Transfer-Encoding", MSG);
			//setLastStatus(E_LENGTH_REQUIRED);
			return false;
		}
	}
	return true;
}

bool RequestProcessor::validateDeleteRequest() {
	// DELETE typically shouldn't have a body, but it's not forbidden
	return true;
}

void RequestProcessor::processGetRequest(MessageValidator* validator) {

	if (!validateGetRequest(validator)) {
		console::log("[ERROR] Invalid GET request", MSG);
		return ;
	}
	// Implementation for GET request processing

	/* 
	if (is_directory(path)) {
		std::map<std::string, std::string> loc_conf = validator->getLocationConfig();
		std::string index = loc_conf["index"];
		if (!index.empty()) {
			// serve index file
			// path = resolve_index_file(path, index);
			return serve_index_file();
		}
		else if (is_autoindex_on()) {
			// generate directory listing
			return generate_directory_listing();
		}
		else {
			// Directory access forbidden
			console::log("[ERROR] Forbidden file location", MSG);
			_last_status = E_FORBIDDEN;
		}

		if (!is_valid_file_path(path)) {
			_last_status = E_NOT_FOUND;
			std::cout << RED << "[ERROR] Invalid file path: " + path << RESET << std::endl;
			return false;
		}
	}
	 */

	console::log("[INFO] Processing GET request for URI: " + _request.getUri().getPath(), MSG);
	// TODO: Implement GET logic (file serving, directory listing, etc.)
}

void RequestProcessor::processPostRequest() {

	if (!validatePostRequest()) {
		console::log("[ERROR] Invalid GET request", MSG);
		return ;
	}
	// Implementation for POST request processing
	console::log("[INFO] Processing POST request for URI: " + _request.getUri().getPath(), MSG);
	// TODO: Implement POST logic (form handling, file upload, CGI, etc.)
}

void RequestProcessor::processDeleteRequest() {

	if (!validateDeleteRequest()) {
		console::log("[ERROR] Invalid GET request", MSG);
		return ;
	}
	// Implementation for DELETE request processing
	console::log("[INFO] Processing DELETE request for URI: " + _request.getUri().getPath(), MSG);
	// TODO: Implement DELETE logic (file deletion, etc.)
}