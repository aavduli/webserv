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
	if (!validateContentLength()) return false;
	if (!validateContentType()) return false;
	if (!validateHeaderLimits()) return false;
	if (!validateRedirection()) return false;
	if (_last_status == E_INIT)
		_last_status = E_OK;
	console::log("[INFO][VALIDATION] Request valid		OK", MSG);

	return true;
}

bool RequestValidator::validateVersion() {

	std::ostringstream oss;
	oss << _request->getHttpVersionMajor() << "." << _request->getHttpVersionMinor();
	const std::string& version = oss.str();
	if (version != "1.1" && version != "1.0") {
		console::log("[ERROR][VALIDATION] Unsupported HTTP version", MSG);
		_last_status = E_UNSUPPORTED_VERSION;
		return false;
	}
	return true;
}

bool RequestValidator::validateHost() {

	RequestUri	uri = _request->getUri();
	std::string config_host = _config.getHost();

	if (_request->getHttpVersionMajor() == "1" && _request->getHttpVersionMinor() == "1") {
		const std::vector<std::string>& h_values = _request->getHeaderValues("Host");
		if (h_values.empty()) {
			console::log("[ERROR][POST] Missing \"Host\" header", MSG);
			_last_status = E_BAD_REQUEST;
			return false;
		}
	}
	
	if (uri.getHost().empty() && !_host_header.empty()) {
		std::string tmp_host = _host_header.at(0);
		size_t colon = tmp_host.find(":");
		if (colon != std::string::npos)
			uri.setHost(tmp_host.substr(0, colon));
		else
			uri.setHost(tmp_host);
	}
	//here may be the probleme
	std::string uri_host = uri.getHost();
	if (uri_host.empty())
		uri.setHost(config_host);
	else {
		if (!uri_host.compare("localhost"))
			uri_host = "127.0.0.1";
		if (uri_host.compare(config_host) && uri_host.compare(_config.getServerName())) {
			console::log("[ERROR][VALIDATION] Invalid Host", MSG);
			console::log("[URI][HOST] " + uri_host, MSG);
			_last_status = E_BAD_REQUEST;
			return false;
		}
	}
	_request->setUri(uri);
	return true;
}

bool RequestValidator::validatePort() {

	RequestUri	uri = _request->getUri();
	std::vector<int> all_ports = _config.getAllPorts();

	if (!_host_header.empty()) {
		std::string host_header = _host_header.at(0);
		size_t colon = host_header.find(":");
		if (colon != std::string::npos) {
			std::string header_port = host_header.substr(colon + 1);
			//maybe other problem there
			char* endptr;
			long port_value = std::strtol(header_port.c_str(), &endptr, 10);
			if (*endptr != '\0' || port_value < 1 || port_value > 65535) {
				console::log("[ERROR][VALIDATION] Invalid port value", MSG);
				_last_status = E_BAD_REQUEST;
				return false;
			}
			
			// Check if port exists in any of the configured servers
			bool port_found = false;
			for (size_t i = 0; i < all_ports.size(); i++) {
				if (all_ports[i] == static_cast<int>(port_value)) {
					port_found = true;
					break;
				}
			}
			
			if (!port_found) {
				console::log("[ERROR][VALIDATION] Request port doesn't match any server config", MSG);
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
	console::log("[ERROR][VALIDATION] Invalid request method", MSG);
	_last_status = E_METHOD_NOT_ALLOWED;
	return false;
}

bool RequestValidator::validatePath() {

	RequestUri	uri = _request->getUri();
	const std::string& path = uri.getPath();

	if (contains_unsafe_chars(path)) {
		console::log("[ERROR][VALIDATION] URI path attempts traversal", MSG);
		_last_status = E_BAD_REQUEST;
		return false;
	}

	std::string relative_path = remove_prefix(path, _request->ctx._location_name);
	std::string full_path = build_full_path(_request->ctx._document_root, relative_path);
	std::string final_path  = canonicalize_path(full_path);

	bool is_upload_post = (_request->getMethod() == "POST" && _request->ctx._upload_enabled);
	
	if (!is_valid_path(final_path)) {
		if (!is_upload_post) {
			console::log("[ERROR][VALIDATION] Invalid path: " + final_path, MSG);
			_last_status = E_NOT_FOUND;
			return false;
		}
		console::log("[INFO][VALIDATION] Upload POST to non-existent path, will be created", MSG);
		final_path = _request->ctx._upload_dir;
	}
	
	if (!is_within_root(final_path, _request->ctx._document_root)) {
		console::log("[ERROR][VALIDATION] URI path escapes document root", MSG);
		_last_status = E_FORBIDDEN;
		return false;
	}
	uri.setEffectivePath(final_path);
	_request->setUri(uri);
	return true;
}

bool RequestValidator::validateContentLength() {

	size_t body_size = _request->getBody().size();
	if (_request->getMethod() == "GET" && body_size == 0)
		return true;

	size_t client_max_body_size = _config.getMaxContentLength();
	if (body_size > client_max_body_size) {
		console::log("[ERROR][VALIDATION] Body size too large", MSG);
		_last_status = E_PAYLOAD_TOO_LARGE;
		return false;
	}

	const std::vector<std::string>& content_length = _request->getHeaderValues("Content-Length");
	if (_request->getMethod() == "POST" && (content_length.empty() || content_length[0].empty())) {
		console::log("[ERROR][REQUEST PARSER] Missing \"Content-Length\" header", MSG);
		_last_status = E_LENGTH_REQUIRED;
		return false;
	}
	if (!content_length.empty()) {
		size_t cl = to_size_t(content_length[0]);
		if (cl != body_size) {
			console::log("[ERROR][REQUEST PARSER] Invalid \"Content-Length\" header value " + content_length[0], MSG);
			_last_status = E_BAD_REQUEST;
			return false;
		}
	}
	return true;
}

bool RequestValidator::validateContentType() {

	if (_request->getMethod() == "POST") {
		const std::vector<std::string>& ct_values = _request->getHeaderValues("Content-Type");
		if (ct_values.empty()) {
			console::log("[ERROR][POST] Missing \"Content-Length\" header", MSG);
			_last_status = E_BAD_REQUEST;
			return false;
		}
	}
	return true;
}

bool RequestValidator::validateHeaderLimits() {

	size_t headers_size = _request->getHeadersSize();
	if (headers_size > MAX_HEADERS_SIZE) {
		console::log("[ERROR][VALIDATION] Header size too large", MSG);
		_last_status = E_HEADER_TOO_LARGE;
		return false;
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
	else if (code == "302")
		_last_status = E_REDIRECT_TEMPORARY;
	_request->setUri(uri);
	return true;
}
