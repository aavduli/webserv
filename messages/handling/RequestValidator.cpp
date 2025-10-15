#include "RequestValidator.hpp"
#include <cstring>

RequestValidator::RequestValidator(const WebservConfig& config, HttpRequest* request) : _config(config), _request(request), _last_status(E_INIT) {
	if (_request) {
		_host_header = _request->getHeaderValues("Host");
	}
}
RequestValidator::RequestValidator(const RequestValidator& rhs) : _config(rhs._config), _request(rhs._request), _last_status(rhs._last_status), _host_header(rhs._host_header) {}
RequestValidator& RequestValidator::operator=(const RequestValidator& rhs) {
	if (this != &rhs) {
		_request = rhs._request;
		_last_status = rhs._last_status;
		_host_header = rhs._host_header;
	}
	return *this;
}
RequestValidator::~RequestValidator() {}

Status RequestValidator::getLastStatus() const {return _last_status;}

bool RequestValidator::validateRequest() {

	if (!validateVersion()) return false;
	if (!validateHost()) return false;
	if (!validatePort()) return false;
	if (!validateMethod()) return false;
	if (!validatePath()) return false;
	if (!validateContentType()) return false;
	if (!validateBodySize()) return false;
	if (!validateHeaderLimits()) return false;
	if (!validateConnectionHeader()) return false;
	if (!validateRedirection()) return false;
	return true;
}

bool RequestValidator::validateVersion() {

	std::ostringstream oss;
	oss << _request->getHttpVersionMajor() << "." << _request->getHttpVersionMinor();
	const std::string& version = oss.str();
	if (version != "1.1" && version != "1.0" && version != "0.9") {
		console::log("[ERROR][REQUEST VALIDATOR] Unsupported HTTP version", MSG);
		_last_status = E_UNSUPPORTED_VERSION;
		return false;
	}
	return true;
}

bool RequestValidator::validateHost() {

	RequestUri	uri = _request->getUri();
	std::string config_host = _config.getHost();
	
	if (uri.getHost().empty() && !_host_header.empty()) {
		std::string tmp_host = _host_header.at(0);
		size_t colon = tmp_host.find(":");
		if (colon != std::string::npos)
			uri.setHost(tmp_host.substr(0, colon));
		else
			uri.setHost(tmp_host);
	}
	if (!uri.getHost().empty() && uri.getHost().compare(config_host) && uri.getHost().compare(_config.getServerName())) {
		console::log("[ERROR][REQUEST VALIDATOR] Invalid Host", MSG);
		_last_status = E_BAD_REQUEST;
		return false;
	}
	else if (uri.getHost().empty())
		uri.setHost(config_host);
	_request->setUri(uri);
	return true;
}

bool RequestValidator::validatePort() {

	RequestUri	uri = _request->getUri();
	int config_port = _config.getPort();

	if (!_host_header.empty()) {
		std::string host_header = _host_header.at(0);
		size_t colon = host_header.find(":");
		if (colon != std::string::npos) {
			std::string header_port = host_header.substr(colon + 1);

			char* endptr;
			long port_value = std::strtol(header_port.c_str(), &endptr, 10);
			if (*endptr != '\0' || port_value < 1 || port_value > 65535) {
				console::log("[ERROR][REQUEST VALIDATOR] Invalid port value", MSG);
				_last_status = E_BAD_REQUEST;
				return false;
			}
			if (static_cast<int>(port_value) != config_port) {
				console::log("[ERROR][REQUEST VALIDATOR] Request port doesn't match server config", MSG);
				_last_status = E_BAD_REQUEST;
				return false;
			}
			uri.setPort(header_port);
		}
	}
	_request->setUri(uri);
	return true;
}

bool RequestValidator::validateMethod() {

	const std::string& method = _request->getMethod();
	std::vector<std::string> allowed_methods;
	std::map<std::string, std::string> config = _request->ctx._location_config;

	if (!config.empty() && !config["methods"].empty())
		allowed_methods = str_to_vect(config["methods"], " ");
	else
		allowed_methods = _config.getAllowedMethods();

	std::vector<std::string>::const_iterator it;
	for (it = allowed_methods.begin(); it != allowed_methods.end(); ++it) {
		if (method == *it)
			return true;
	}
	console::log("[ERROR][REQUEST VALIDATOR] Invalid request method", MSG);
	_last_status = E_METHOD_NOT_ALLOWED;
	return false;
}

bool RequestValidator::validatePath() {

	RequestUri	uri = _request->getUri();
	const std::string& path = uri.getPath();

	if (contains_traversal(path)) {
		console::log("[ERROR][REQUEST VALIDATOR] URI path attempts traversal", MSG);
		_last_status = E_BAD_REQUEST;
		return false;
	}

	std::string relative_path = remove_prefix(path, _request->ctx._location_name);
	std::string full_path = build_full_path(_request->ctx._document_root, relative_path);
	std::string final_path  = canonicalize_path(full_path);

	if (!is_valid_path(final_path)) {
		console::log("[ERROR][REQUEST VALIDATOR] Invalid path: " + final_path, MSG);
		_last_status = E_NOT_FOUND;
		return false;
	}
	if (!is_within_root(final_path, _request->ctx._document_root)) {
		console::log("[ERROR][REQUEST VALIDATOR] URI path escapes document root", MSG);
		_last_status = E_BAD_REQUEST;
		return false;
	}
	uri.setEffectivePath(final_path);
	_request->setUri(uri);
	return true;
}

// POST always carries data, so Content-Type is mandatory and helps the server know how to parse and handle the request body
bool RequestValidator::validateContentType() {

	const std::string& method = _request->getMethod();
	
	if (method == "POST") {
		const std::vector<std::string>& ct_headers = _request->getHeaderValues("Content-Type");
		if (ct_headers.empty()) {
			console::log("[ERROR][REQUEST VALIDATOR] POST method requires Content-Type header", MSG);
			_last_status = E_BAD_REQUEST;
			return false;
		}

		// Basic content type validation (extend based on location config) TODO
		std::string content_type = ct_headers[0];
		if (content_type.find("application/") == std::string::npos &&
			content_type.find("text/") == std::string::npos &&
			content_type.find("multipart/") == std::string::npos) {
			console::log("[ERROR][REQUEST VALIDATOR] Unsupported content type: " + content_type, MSG);
			_last_status = E_UNSUPPORTED_MEDIA_TYPE;
			return false;
		}
	}
	return true;
}

bool RequestValidator::validateBodySize() {

	size_t config_max = _config.getMaxContentLength();
	size_t body_size = _request->getBody().size();

	if (body_size > config_max) {
		console::log("[ERROR][REQUEST VALIDATOR] Body size too large", MSG);
		_last_status = E_PAYLOAD_TOO_LARGE;
		return false;
	}
	return true;
}

bool RequestValidator::validateHeaderLimits() {

	size_t headers_size = _request->getHeadersSize();
	if (headers_size > MAX_HEADERS_SIZE) {
		console::log("[ERROR][REQUEST VALIDATOR] Header size too large", MSG);
		_last_status = E_HEADER_TOO_LARGE;
		return false;
	}
	return true;
}

// HTTP/1.0 doesnt handle 'upgrade' value and connections are closed by default
bool RequestValidator::validateConnectionHeader() {

	const std::vector<std::string>& conn_headers = _request->getHeaderValues("Connection");
	if (conn_headers.empty())
		return true;

	for (size_t i = 0; i < conn_headers.size(); i++) {
		std::string conn = conn_headers[i];
		if (conn != "close" && conn != "keep-alive") {
			console::log("[ERROR] Invalid \"Connection\" header value: " + conn, MSG);
			_last_status = E_BAD_REQUEST;
			return false;
		}
	}
	return true;
}

bool RequestValidator::validateRedirection() {

	std::map<std::string, std::string> config = _request->ctx._location_config;
	const std::string& redirect = config["return"];
	if (redirect.empty())
		return true;

	size_t space = redirect.find(' ');
	std::string code = redirect.substr(0, space);
	std::string destination = redirect.substr(space + 1);

	RequestUri	uri = _request->getUri();
	uri.setRedirDestination(destination);
	_request->ctx._has_redirect = true;
	if (code == "301")
		_last_status = E_REDIRECT_PERMANENT;
	else
		_last_status = E_REDIRECT_TEMPORARY;
	_request->setUri(uri);
	return true;
}