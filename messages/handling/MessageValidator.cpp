#include "MessageValidator.hpp"
#include <cstring>

MessageValidator::MessageValidator(const WebservConfig& config, HttpRequest& request) : _config(config), _request(request), _last_status(E_OK) {
	_host_header = _request.getHeaderValues("host");
	if (_host_header.empty()) {
		_last_status = E_EMPTY_HEADER_HOST;
		return ;
	}
	std::string path = _request.getUri().getPath();
	_location_config = findLocationMatch(path);
}

bool MessageValidator::isValidRequest() {
	_last_status = E_OK;
	
	if (!validateVersion()) return false;	// 505 HTTP VERSION NOT SUPPORTED
	if (!validateHost()) return false;		// 400 BAD REQUEST
	if (!validatePort()) return false;		// 400 BAD REQUEST
	if (!validateMethod()) return false;	// 405 METHOD NOT ALLOWED
	if (!validatePath()) return false;		// 404 NOT FOUND / 400 BAD REQUEST
	if (!validateTransferEncoding()) return false;	// 400 BAD REQUEST
	if (!validateContentType()) return false;		// 400 BAD REQUEST / 415 UNSUPPORTED MEDIA TYPE
	if (!validateBodySize()) return false;	// 413 PAYLOAD TOO LARGE
	if (!validateExpectHeader()) return false;		// 417 EXPECTATION FAILED
	if (!validateHeaderLimits()) return false;		// 400 BAD REQUEST
	if (!validateConnectionHeader()) return false;	// 400 BAD REQUEST
	if (!validateRedirection()) return false;		// 301/302 REDIRECT
	return true;
}

Status MessageValidator::getLastStatus() const {return _last_status;}

// TODO Validation format hostname (RFC 1123)
bool MessageValidator::validateHost() {

	std::string config_host = _config.getHost();
	
	if (_request.getUri().getHost().empty()) {
		std::string tmp_host = _host_header.at(0);
		size_t colon = tmp_host.find(":");
		if (colon != std::string::npos)
			_request.getUri().setHost(tmp_host.substr(0, colon));
		else
			_request.getUri().setHost(tmp_host);
	}
	if (!_request.getUri().getHost().empty() && _request.getUri().getHost().compare(config_host)) {
		_last_status = E_INVALID_HOST;
		return false;
	}
	else if (_request.getUri().getHost().empty())
		_request.getUri().setHost(config_host);
	return true;
}

// TODO Gestion des ports réservés
bool MessageValidator::validatePort() {

	int config_port = _config.getPort();

	if (!_host_header.empty()) {
		std::string host_header = _host_header.at(0);
		size_t colon = host_header.find(":");
		if (colon != std::string::npos) {
			std::string header_port = host_header.substr(colon + 1);

			char* endptr;
			long port_value = std::strtol(header_port.c_str(), &endptr, 10);
			if (*endptr != '\0' || port_value < 1 || port_value > 65535) {
				console::log("[ERROR] Invalid port number: " + header_port, MSG);
				_last_status = E_INVALID_PORT;
				return false;
			}
			
			if (static_cast<int>(port_value) != config_port) {
				console::log("[ERROR] Request port doesn't match server port", MSG);
				_last_status = E_INVALID_PORT;
				return false;
			}
			_request.getUri().setPort(header_port);
		}
	}
	return true;
}

bool MessageValidator::validateMethod() {

	const std::string& method = _request.getMethod();
	std::vector<std::string> allowed_methods;
	
	if (!_location_config.empty() && !_location_config["methods"].empty())
		allowed_methods = str_to_vect(_location_config["methods"], " ");
	else
		allowed_methods = _config.getAllowedMethods();

	for (std::vector<std::string>::const_iterator it = allowed_methods.begin(); it != allowed_methods.end(); ++it) {
		if (method == *it)
			return true;
	}
	_last_status = E_METHOD_NOT_ALLOWED;
	console::log("[ERROR] Method not allowed " + method, MSG);
	return false;
}

// need default version MACRO?
bool MessageValidator::validateVersion() {

	const std::string& version = _request.getHttpVersion();
	if (version != "1.1" && version != "1.0" && version != "0.9") {
		_last_status = E_UNSUPPORTED_VERSION;
		return false;
	}
	return true;
}

bool MessageValidator::validateBodySize() {

	size_t config_max = _config.getMaxContentLength();
	size_t body_size = _request.getBodySize();

	if (body_size > config_max) {
		_last_status = E_ENTITY_TOO_LARGE;
		return false;
	}
	return true;
}

bool MessageValidator::validatePath() {

	const std::string& path = _request.getUri().getPath();
	std::string root = _location_config["root"];

	if (contains_traversal(path)) {
		_last_status = E_PATH_TRAVERSALS;
		return false;
	}
	if (root.empty())
		root = _config.getRoot();
	
	std::string relative_path = extract_relative_path(path, _location_prefix);
	std::string full_path = build_full_path(root, relative_path);
	std::string final_path  = canonicalize_path(full_path);

	if (!is_valid_path(final_path)) {
		_last_status = E_NOT_FOUND;
		return false;
	}

	if (is_directory(final_path)) {
		std::string index = _location_config["index"];
		if (index.empty())
			index = _config.getIndex();
		final_path = resolve_index_file(final_path, index);

		if (!is_valid_file_path(final_path)) {
			_last_status = E_NOT_FOUND;
			return false;
		}
	}
	if (!is_within_root(final_path, root)) {
		_last_status = E_PATH_ESCAPES_ROOT;
		return false;
	}
	_request.getUri().setEffectivePath(final_path);
	std::cout << YELLOW << "[INFO] Effective path: " + final_path << RESET << std::endl;
	return true;
}

// Transfer-Encoding validation (RFC 7230)
// Transfer-Encoding is used for chunked data transmission where the body size is unknown
// Example: "Transfer-Encoding: chunked" - data sent in chunks with size prefixes
bool MessageValidator::validateTransferEncoding() {

	const std::vector<std::string>& te_headers = _request.getHeaderValues("transfer-encoding");
	if (te_headers.empty())
		return true;

	// Check for unsupported encodings - only "chunked" is supported
	// Other encodings like "gzip", "deflate" are not implemented
	for (size_t i = 0; i < te_headers.size(); i++) {
		if (te_headers[i] != "chunked") {
			console::log("[ERROR] Unsupported transfer encoding: " + te_headers[i], MSG);
			_last_status = E_INVALID_TRANSFER_ENCODING;
			return false;
		}
	}

	// RFC 7230: If both Transfer-Encoding and Content-Length present, ignore Content-Length
	// This is because chunked encoding makes Content-Length meaningless
	// The body size is determined by the chunk sizes, not by Content-Length
	if (_request.hasHeader("content-length")) {
		console::log("[WARNING] Both Transfer-Encoding and Content-Length present, ignoring Content-Length", MSG);
		
		// CRITICAL: Signal to handler that Content-Length must be ignored
		// Handler MUST use chunked reading instead of fixed-length reading
		// This prevents reading wrong amount of data or parsing errors
	}
	
	// Signal to handler that chunked transfer encoding is active
	// Handler must implement chunked reading: size\r\ndata\r\n...0\r\n\r\n
	console::log("[INFO] Chunked transfer encoding detected, handler will read chunks", MSG);
	return true;
}

// POST always carries data, so Content-Type is mandatory and helps the server know how to parse and handle the request body
bool MessageValidator::validateContentType() {

	const std::string& method = _request.getMethod();
	
	if (method == "POST") {
		const std::vector<std::string>& ct_headers = _request.getHeaderValues("content-type");
		if (ct_headers.empty()) {
			console::log("[ERROR] POST request requires Content-Type header", MSG);
			_last_status = E_MISSING_CONTENT_TYPE;
			return false;
		}

		// Basic content type validation (extend based on location config) TODO
		std::string content_type = ct_headers[0];
		if (content_type.find("application/") == std::string::npos &&
			content_type.find("text/") == std::string::npos &&
			content_type.find("multipart/") == std::string::npos) {
			console::log("[ERROR] Unsupported content type: " + content_type, MSG);
			_last_status = E_UNSUPPORTED_MEDIA_TYPE;
			return false;
		}
	}
	return true;
}

bool MessageValidator::validateHeaderLimits() {

	size_t headers_size = _request.getHeadersSize();
	if (headers_size > MAX_HEADERS_SIZE) {
		console::log("[ERROR] Header size too large", MSG);
		_last_status = E_ENTITY_TOO_LARGE;
		return false;
	}
	return true;
}

/*	A server that receives a 100-continue expectation in an HTTP/1.0 request MUST ignore that expectation.
	Upon receiving an HTTP/1.1 (or later) request that contains a 100-continue expectation, an origin server MUST send either:
		- an immediate response with a final status code, if that status can be determined by examining just the method, target URI, and header fields, or
		- an immediate 100 (Continue) response to encourage the client to send the request content.
*/
bool MessageValidator::validateExpectHeader() {

	const std::vector<std::string>& expect_headers = _request.getHeaderValues("expect");
	if (expect_headers.empty())
		return true;

	const std::string& version = _request.getHttpVersion();
	if (version == "1.0" || version == "0.9") {
		console::log("[INFO] Ignoring Expect header for HTTP/" + version + " request (RFC compliant)", MSG);
		return true;
	}
	for (size_t i = 0; i < expect_headers.size(); i++) {
		if (expect_headers[i] != "100-continue") {
			console::log("[ERROR] Unsupported expectation: " + expect_headers[i], MSG);
			_last_status = E_EXPECTATION_FAILED;
			return false;
		}
	}

	// If we reach here with valid expectations, signal to handler that 100-continue is needed
	// The handler should send "100 Continue" before reading body content
	console::log("[INFO] Valid 100-continue expectation detected, handler should send 100 Continue", MSG);
	return true;
}

// Controls whether connection should be kept alive or closed after response (RFC 7230)
// Essential for HTTP/1.1 persistent connections and connection management
bool MessageValidator::validateConnectionHeader() {

	const std::vector<std::string>& conn_headers = _request.getHeaderValues("connection");
	if (conn_headers.empty())
		return true;

	for (size_t i = 0; i < conn_headers.size(); i++) {
		std::string conn = conn_headers[i];
		if (conn != "close" && conn != "keep-alive" && conn != "upgrade") {
			console::log("[ERROR] Invalid \"Connection\" header value: " + conn, MSG);
			_last_status = E_INVALID_CONNECTION;
			return false;
		}
	}
	return true;
}

// Return false to stop processing, but it's a valid redirect if 301 or 302
bool MessageValidator::validateRedirection() {

	const std::string& redirect = _location_config["return"];
	if (redirect.empty())
		return true;

	size_t space = redirect.find(' ');
	if (space == std::string::npos) {
		console::log("[ERROR] Invalid redirect format: " + redirect, MSG);
		_last_status = E_INVALID_REDIRECT;
		return false;
	}

	std::string code = redirect.substr(0, space);
	if (code != "301" && code != "302") {
		console::log("[ERROR] Unsupported redirect code: " + code, MSG);
		_last_status = E_INVALID_REDIRECT;
		return false;
	}
	
	std::string destination = redirect.substr(space + 1);
	_request.getUri().setRedirDestination(destination);

	if (code == "301")
		_last_status = E_REDIRECT_PERMANENT;
	else
		_last_status = E_REDIRECT_TEMPORARY;
	console::log("[INFO] Redirect " + code + " to '" + destination + "' detected for path: " + _request.getUri().getPath(), MSG);
	return false;
}

// Uses longest prefix matching
std::map<std::string, std::string> MessageValidator::findLocationMatch(const std::string& path) {
	
	std::string match = "";
	std::map<std::string, std::string> best_config;
	_location_prefix = "";
	
	if (_config.hasLocation(path))
		return _config.getLocationConfig(path);
	
	std::string test_path = path;
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
	_location_prefix = match;
	return best_config;
}
