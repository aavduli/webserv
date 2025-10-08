#include "RequestValidator.hpp"
#include <cstring>

RequestValidator::RequestValidator(const WebservConfig& config, HttpRequest* request) : _config(config) {
	 _request = request;
	 _last_status = E_INIT;
	_host_header = _request->getHeaderValues("host");
	if (_host_header.empty()) {
		_last_status = E_EMPTY_HEADER_HOST;
		return ;
	}
}
RequestValidator::RequestValidator(const RequestValidator& rhs) : _config(rhs._config), _request(rhs._request), _last_status(rhs._last_status) {}
RequestValidator& RequestValidator::operator=(const RequestValidator& rhs) {
	if (this != &rhs) {
		_request = rhs._request;
		_last_status = rhs._last_status;
	}
	return *this;
}
RequestValidator::~RequestValidator() {}

Status RequestValidator::getLastStatus() const {return _last_status;}

bool RequestValidator::validateRequest() {
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

// TODO Validation format hostname (RFC 1123)
bool RequestValidator::validateHost() {

	RequestUri	uri = _request->getUri();
	std::string config_host = _config.getHost();
	
	if (uri.getHost().empty()) {
		std::string tmp_host = _host_header.at(0);
		size_t colon = tmp_host.find(":");
		if (colon != std::string::npos)
			uri.setHost(tmp_host.substr(0, colon));
		else
			uri.setHost(tmp_host);
	}
	if (!uri.getHost().empty() && uri.getHost().compare(config_host) && uri.getHost().compare(_config.getServerName())) {
		_last_status = E_INVALID_HOST;
		return false;
	}
	else if (uri.getHost().empty())
		uri.setHost(config_host);
	_request->setUri(uri);
	return true;
}

// TODO Gestion des ports réservés?
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
				console::log("[ERROR] Invalid port number: " + header_port, MSG);
				_last_status = E_INVALID_PORT;
				return false;
			}
			
			if (static_cast<int>(port_value) != config_port) {
				console::log("[ERROR] Request port doesn't match server port", MSG);
				_last_status = E_INVALID_PORT;
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
	std::map<std::string, std::string> config = _request->ctx.getLocationConfig();

	if (!config.empty() && !config["methods"].empty())
		allowed_methods = str_to_vect(config["methods"], " ");
	else
		allowed_methods = _config.getAllowedMethods();

	for (std::vector<std::string>::const_iterator it = allowed_methods.begin(); it != allowed_methods.end(); ++it) {
		if (method == *it)
			return true;
	}
	_last_status = E_METHOD_NOT_ALLOWED;
	return false;
}

// need default version MACRO?
bool RequestValidator::validateVersion() {

	const std::string& version = _request->getHttpVersion();
	if (version != "1.1" && version != "1.0" && version != "0.9") {
		_last_status = E_UNSUPPORTED_VERSION;
		return false;
	}
	return true;
}

bool RequestValidator::validateBodySize() {

	size_t config_max = _config.getMaxContentLength();
	size_t body_size = _request->getBodySize();

	if (body_size > config_max) {
		_last_status = E_ENTITY_TOO_LARGE;
		console::log("[ERROR] Body size too large", MSG);
		return false;
	}
	return true;
}

bool RequestValidator::validatePath() {

	RequestUri	uri = _request->getUri();
	const std::string& path = uri.getPath();
	if (contains_traversal(path)) {
		_last_status = E_PATH_TRAVERSALS;
		return false;
	}
	
	std::string relative_path = extract_relative_path(path, _request->ctx.getLocationName());
	std::string full_path = build_full_path(_request->ctx.getDocumentRoot(), relative_path);
	std::string final_path  = canonicalize_path(full_path);

	if (!is_valid_path(final_path)) {
		_last_status = E_NOT_FOUND;
		console::log("[ERROR] Invalid path: " + final_path, MSG);
		return false;
	}
	if (!is_within_root(final_path, _request->ctx.getDocumentRoot())) {
		_last_status = E_PATH_ESCAPES_ROOT;
		return false;
	}
	uri.setEffectivePath(final_path);
	_request->setUri(uri);
	return true;
}

// Transfer-Encoding validation (RFC 7230)
// Transfer-Encoding is used for chunked data transmission where the body size is unknown
// Example: "Transfer-Encoding: chunked" - data sent in chunks with size prefixes
bool RequestValidator::validateTransferEncoding() {

	const std::vector<std::string>& te_headers = _request->getHeaderValues("transfer-encoding");
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
	if (_request->hasHeader("content-length")) {
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
bool RequestValidator::validateContentType() {

	const std::string& method = _request->getMethod();
	
	if (method == "POST") {
		const std::vector<std::string>& ct_headers = _request->getHeaderValues("content-type");
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

bool RequestValidator::validateHeaderLimits() {

	size_t headers_size = _request->getHeadersSize();
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
bool RequestValidator::validateExpectHeader() {

	const std::vector<std::string>& expect_headers = _request->getHeaderValues("expect");
	if (expect_headers.empty())
		return true;

	const std::string& version = _request->getHttpVersion();
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
bool RequestValidator::validateConnectionHeader() {

	const std::vector<std::string>& conn_headers = _request->getHeaderValues("connection");
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
bool RequestValidator::validateRedirection() {

	std::map<std::string, std::string> config = _request->ctx.getLocationConfig();
	const std::string& redirect = config["return"];
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
	
	RequestUri	uri = _request->getUri();
	std::string destination = redirect.substr(space + 1);
	uri.setRedirDestination(destination);
	_request->ctx.setRedirect(true);

	if (code == "301")
		_last_status = E_REDIRECT_PERMANENT;
	else
		_last_status = E_REDIRECT_TEMPORARY;
	_request->setUri(uri);
	console::log("[INFO] Redirect " + code + " to '" + destination + "' detected for path: " + _request->getUri().getPath(), MSG);
	return true;
}
