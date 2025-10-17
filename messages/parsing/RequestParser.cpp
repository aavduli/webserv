#include "RequestParser.hpp"

RequestParser::RequestParser(const WebservConfig& config, HttpRequest* request, const std::string& raw_request) : _config(config), _request(request), _raw_request(raw_request), _current_pos(0), _last_status(E_INIT) {}
RequestParser::RequestParser(const RequestParser& rhs) : _config(rhs._config), _request(rhs._request), _raw_request(rhs._raw_request), _current_pos(rhs._current_pos), _last_status(rhs._last_status) {}
RequestParser& RequestParser::operator=(const RequestParser& rhs) {
	if (this != &rhs) {
		_request = rhs._request;
		_raw_request = rhs._raw_request;
		_current_pos = rhs._current_pos;
		_last_status = rhs._last_status;
	}
	return *this;
}
RequestParser::~RequestParser() {}

Status RequestParser::getLastStatus() const {return _last_status;}

bool RequestParser::parseRequest() {

	if (!parseRequestLine()) {
		console::log("[DEBUG] Failed to parse request line", MSG);
		return false;
	}
	if (!parseHeaders()) {
		console::log("[DEBUG] Failed to parse headers", MSG);
		return false;
	}
	if (!parseBody()) {
		console::log("[DEBUG] Failed to parse body", MSG);
		return false;
	}
	return true;
}

bool RequestParser::parseRequestLine() {

	size_t line_end = _raw_request.find("\r\n", _current_pos);
	if (line_end == std::string::npos) {
		console::log("[ERROR][REQUEST PARSER] No CRLF found in request line", MSG);
		_last_status = E_UNSUPPORTED_VERSION;
		return false;
	}
	std::string request_line = _raw_request.substr(_current_pos, line_end - _current_pos);
	request_line = trim_lws(request_line);
	
	if (!parseMethod(request_line))
		return false;
	if (!parseUri(request_line))
		return false;
	if (!parseVersion(request_line))
		return false;
	_current_pos = line_end + 2;
	return true;
}

bool RequestParser::parseMethod(std::string request_line) {

	size_t method_end = request_line.find(" ", _current_pos);
	if (method_end == std::string::npos) {
		console::log("[ERROR][REQUEST PARSER] No SP found after method", MSG);
		_last_status = E_NOT_IMPLEMENTED;
		return false;
	}
	std::string method = request_line.substr(_current_pos, method_end - _current_pos);
	if (method.empty()) {
		console::log("[ERROR][REQUEST PARSER] No request method found", MSG);
		_last_status = E_NOT_IMPLEMENTED;
		return false;
	}
	for (size_t i = 0; i < method.size(); i++) {
		if (!is_token(method[i])) {
			console::log("[ERROR][REQUEST PARSER] Invalid token in method name", MSG);
			_last_status = E_NOT_IMPLEMENTED;
			return false;
		}
	}
	_request->setMethod(method);
	_current_pos = request_line.find_first_not_of(" ", method_end);
	return true;
}

bool RequestParser::parseUri(std::string request_line) {

	size_t uri_end = request_line.find(" ", _current_pos);
	if (uri_end == std::string::npos) {
		console::log("[ERROR][REQUEST PARSER] No SP found after URI", MSG);
		_last_status = E_NOT_FOUND;
		return false;
	}
	std::string raw_uri = request_line.substr(_current_pos, uri_end - _current_pos);
	raw_uri = trim_whitespaces(raw_uri);
	if (raw_uri.empty()) 
		raw_uri = "/";
	if (raw_uri.length() > MAX_URI_LENGTH) {
		console::log("[ERROR][REQUEST PARSER] Request URI too long", MSG);
		_last_status = E_URI_TOO_LONG;
		return false;
	}
	RequestUri uri(raw_uri);
	if (!uri.parse()) {
		console::log("[ERROR][REQUEST PARSER] Invalid URI", MSG);
		_last_status = E_NOT_FOUND;
		return false;
	}
	_request->setUri(uri);
	_current_pos = request_line.find_first_not_of(" ", uri_end);
	return true;
}

bool RequestParser::parseVersion(std::string request_line) {

	size_t version = request_line.find("HTTP/", _current_pos);	// could be https
	if (version != std::string::npos) {
		_current_pos += 5;
		size_t dot = request_line.find_first_of('.', _current_pos);
		std::string major = request_line.substr(_current_pos, dot - _current_pos);
		std::string minor = request_line.substr(dot + 1, request_line.size() - (dot + 1));
		_request->setHttpVersion(major, minor);
		return true;
	}
	console::log("[ERROR][REQUEST PARSER] Invalid HTTP version", MSG);
	_last_status = E_UNSUPPORTED_VERSION;
	return false;
}
 
bool RequestParser::parseHeaders() {

	if (_current_pos < _raw_request.length())
		_request->setHeadersSize(_raw_request.substr(_current_pos).size());

	while (_current_pos < _raw_request.length()) {
		size_t line_end = _raw_request.find("\r\n", _current_pos);
		if (line_end == std::string::npos) {
			console::log("[ERROR][REQUEST PARSER] No CRLF found in headers", MSG);
			_last_status = E_BAD_REQUEST;
			return false;
		}
		std::string header_line = _raw_request.substr(_current_pos, line_end - _current_pos);
		_current_pos = line_end + 2;
		if (header_line.empty())
			break;
		
		std::string name;
		std::vector<std::string> values;
		if (!parseHeaderLine(header_line, name, values)) {
			console::log("[ERROR][REQUEST PARSER] Invalid header line: " + header_line, MSG);
			_last_status = E_BAD_REQUEST;
			return false;
		}
		_request->addHeader(name, values);
	}
	return true;
}

// non-member function used in MultipartData parsing
bool parseHeaderLine(const std::string& header_line, std::string& name, std::vector<std::string>& values) {
	
	if (header_line.empty())
		return false;
	
	size_t colon_pos = header_line.find(':');
	if (colon_pos == std::string::npos)
		return false;
	
	name = trim_lws(header_line.substr(0, colon_pos));
	if (name.empty())
		return false;
	
	std::string value = trim_lws(header_line.substr(colon_pos + 1));
	values.clear();
	
	if (value.find(',') != std::string::npos)
		values = str_to_vect_exept_between(value, ",", "(", ")");
	else
		values.push_back(trim_lws(value));
	
	if (values.empty())
		return false;
	return true;
}

bool RequestParser::parseBody() {

	if (_current_pos < _raw_request.length()) {
		std::string body = _raw_request.substr(_current_pos);
		_request->setBody(body);
	}
	else
		_request->setBody("");
	return true;
}

void	RequestParser::setRequestContext() {

	RequestContext ctx;
	std::string path = _request->getUri().getPath();

	ctx._location_name = findLocationName(path);
	if (ctx._location_name.empty())
		ctx._location_config = _config.getServer();
	else
		ctx._location_config = _config.getLocationConfig(ctx._location_name);

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

	std::string upload_enabled = config["upload_enabled"];
	if (!upload_enabled.empty())
		ctx._upload_enabled = true;
	else
		ctx._upload_enabled = false;

	std::string upload_dir = config["upload_dir"];
	if (!upload_dir.empty())
		ctx._upload_dir = upload_dir;

	_request->ctx = ctx;
}

std::string	RequestParser::findLocationName(const std::string& path) {
	
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
