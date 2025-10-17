#include "RequestProcessor.hpp"

RequestProcessor::RequestProcessor(const WebservConfig& config, HttpRequest* request) : _config(config), _request(request), _last_status(E_INIT) {}
RequestProcessor::RequestProcessor(const RequestProcessor& rhs) : _config(rhs._config), _request(rhs._request), _last_status(rhs._last_status) {}
RequestProcessor& RequestProcessor::operator=(const RequestProcessor& rhs) {
	if (this != &rhs) {
		_request = rhs._request;
		_last_status = rhs._last_status;
	}
	return *this;
}
RequestProcessor::~RequestProcessor() {}

Status RequestProcessor::getLastStatus() const {return _last_status;}

// post can be data sharing, a CGI or a file upload
bool RequestProcessor::processPostRequest() {

	if (!decodePostBody())
		return false;
	if (isFileUpload())
		return processFileUpload();
	else if (isCGI())
		return processCGI();	// TODO what needs to be processed here in addition than in response generation?
	_last_status = E_OK;
	return true;
}

bool	RequestProcessor::decodePostBody() {

	const std::vector<std::string>& ct_values = _request->getHeaderValues("Content-Type");
	if (ct_values.empty()) {
		console::log("[ERROR][POST BODY DECODING] Content-Type empty", MSG);
		_last_status = E_BAD_REQUEST;
		return false;
	}

	std::string content_type = ct_values[0];
	if (content_type.find("application/x-www-form-urlencoded") != std::string::npos) {
		processURLEncodedBody();
	}
	else if (content_type.find("multipart/form-data") != std::string::npos) {
		processMultipartBody();
	}
	else if (content_type.find("text/") != std::string::npos) {
		_last_status = E_OK;
	}
	else {
		console::log("[ERROR][POST BODY DECODING] Invalid Content-Type", MSG);
		_last_status = E_BAD_REQUEST;
	}
	return (_last_status == E_OK);
}

void	RequestProcessor::processURLEncodedBody() {

	std::string raw_body = _request->getBody();
	std::map<std::string, PostValue> post_data;

	std::vector<std::string> pairs = str_to_vect(_request->getBody(), "&");
	for (size_t i = 0; i < pairs.size(); i++) {
		if (pairs[i].empty())
			continue;

		size_t equal = pairs[i].find("=");
		if (equal == std::string::npos) {
			post_data[urlDecode(pairs[i])] = PostValue("");
		}
		else {
			std::string key = urlDecode(pairs[i].substr(0, equal));
			std::string value = urlDecode(pairs[i].substr(equal + 1));
			post_data[key] = PostValue(value);
		}
	}
	_request->setPostData(post_data);
}

std::string	RequestProcessor::urlDecode(const std::string& encoded) {

	std::string	decoded;

	for (size_t i = 0; i < encoded.size(); i++) {

		// %xx where xx is hexadecimal representation of a ASCII char
		if (encoded[i] == '%' && i + 2 < encoded.size()) {
			char hex1 = encoded[i + 1];
			char hex2 = encoded[i + 2];
			
			if (is_hex_digit(hex1) && is_hex_digit(hex2)) {
				int hex_val = (hex_to_int(hex1) << 4) | hex_to_int(hex2);
				decoded += static_cast<char>(hex_val);
				i += 2;
			}
			else
				decoded += encoded[i];
		}
		else if (encoded[i] == '+')
			decoded += ' ';
		else
			decoded += encoded[i];
	}
	return decoded;
}

// TODO atomize
void	RequestProcessor::processMultipartBody() {
	
	std::map<std::string, PostValue> post_data;
	const std::string& raw_body = _request->getBody();
	std::string boundary = extractBoundary(_request->getHeaderValues("Content-Type"));
	
	if (boundary.empty()) {
		console::log("[ERROR] Missing or invalid multipart boundary", MSG);
		_last_status = E_BAD_REQUEST;
		return;
	}

	std::vector<std::string> parts = splitByBoundary(raw_body, boundary);
	if (parts.empty()) {
		console::log("[ERROR] No multipart parts found", MSG);
		_last_status = E_BAD_REQUEST;
		return;
	}
	
	for (size_t i = 0; i < parts.size(); i++) {
		
		size_t header_end = parts[i].find("\r\n\r\n");
		if (header_end == std::string::npos) {
			console::log("[INFO] Malformed multipart part - missing header separator", MSG);
			continue;
		}
		
		std::map<std::string, std::vector<std::string> > headers = parseMultipartHeaders(parts[i].substr(0, header_end));
		
		std::string field_name = extractDispositionData(headers, "name=\"");
		if (field_name.empty()) {
			console::log("[INFO] Multipart: missing name attribute", MSG);
			continue;
		}
		
		PostValue value;
		value.content = parts[i].substr(header_end + 4);
		
		std::string filename = extractDispositionData(headers, "filename=\"");
		if (!filename.empty()) {
			value.filename = filename;
			value.is_file = true;
			if (headers.find("Content-Type") != headers.end() && !headers["Content-Type"].empty())
				value.content_type = headers["Content-Type"][0];
		}
		post_data[field_name] = value;
	}
	_request->setPostData(post_data);
}

std::string	RequestProcessor::extractBoundary(const std::vector<std::string>& ct_values) {

	if (ct_values.empty())
		return "";

	const std::string& content_type = ct_values[0];
	size_t b_pos = content_type.find("boundary=");
	if (b_pos == std::string::npos)
		return "";

	std::string boundary = content_type.substr(b_pos + 9);
	if (!boundary.empty()) {
		if (boundary[0] == '\"') {
			if (boundary.length() >= 2) {
				boundary = boundary.substr(1);
				size_t end_quote = boundary.find("\"");
				if (end_quote != std::string::npos)
					boundary = boundary.substr(0, end_quote);
			}
		}
		size_t semi_colon = boundary.find(";");
		if (semi_colon != std::string::npos)
			boundary = boundary.substr(0, semi_colon);
	}
	return boundary;
}

std::vector<std::string>	RequestProcessor::splitByBoundary(const std::string& body, const std::string& boundary_str) {

	std::string	boundary = "--" + boundary_str;
	std::vector<std::string> raw_parts = str_to_vect(body, boundary);
	std::vector<std::string> parts;
	
	if (raw_parts.size() < 2)
		return parts;

	std::vector<std::string> to_trim;
	to_trim.push_back("\r");
	to_trim.push_back("\n");
	to_trim.push_back(" ");
	
	for (size_t i = 1; i < raw_parts.size(); i++) {
		std::string part = trim(raw_parts[i], to_trim);
		
		// skip empty parts and closing boundary markers
		if (!part.empty() && part != "--")
			parts.push_back(part);
	}
	return parts;
}

std::map<std::string, std::vector<std::string> > RequestProcessor::parseMultipartHeaders(const std::string& header_str) {

	std::map<std::string, std::vector<std::string> >	headers;
	
	if (header_str.empty()) {
		console::log("[DEBUG] Empty multipart header section", MSG);
		return headers;
	}
	
	std::vector<std::string> lines = str_to_vect(header_str, "\r\n");
	for (size_t i = 0; i < lines.size(); i++) {
		if (lines[i].empty())
			continue ;
		
		std::string name;
		std::vector<std::string> values;
		if (parseHeaderLine(lines[i], name, values)) {
			if (!values.empty())
				headers[name] = values;
		}
		else
			console::log("[DEBUG] Failed to parse multipart header: " + lines[i], MSG);
	}
	return headers;
}

std::string RequestProcessor::extractDispositionData(const std::map<std::string, std::vector<std::string> >& headers, const std::string& key) {

	std::map<std::string, std::vector<std::string> >::const_iterator it = headers.find("Content-Disposition");
	if (it == headers.end() || it->second.empty()) {
		console::log("[DEBUG] Content-Disposition header missing in multipart part", MSG);
		return "";
	}
	
	std::string value;
	const std::string& disposition = it->second[0];
	size_t name_pos = disposition.find(key);

	if (name_pos != std::string::npos) {
		name_pos += key.length();
		size_t end_pos = disposition.find("\"", name_pos);
		if (end_pos != std::string::npos && end_pos > name_pos)
			value = disposition.substr(name_pos, end_pos - name_pos);
		else
			console::log("[DEBUG] Empty or malformed disposition attribute: " + key, MSG);
	}
	return value;
}

/*
	Destination path resolution (upload dir or CGI handler)
	Directory creation if upload_dir doesn't exist (optional safety)
	Unique filename generation for uploads

	For uploads:
		Check write permissions on destination directory
		Create file with unique name if needed (avoid overwrites)
		Write received body to disk using write() (non-blocking aware)
		Handle disk space errors

	For CGI execution:
		Set up environment variables (REQUEST_METHOD, CONTENT_LENGTH, CONTENT_TYPE, etc.)
		Pass full body as stdin to CGI process
		fork() child process, execute CGI via execve(), waitpid()
		Read CGI output and response headers
		Handle CGI timeout/crash scenarios

	Destination path resolution:

	Determine upload directory from config
	Generate unique filenames to prevent overwrites
	Validate file extensions against allowed list

	File writing:

	Create upload directory if it doesn't exist
	Write multipart file content to disk
	Handle partial writes and disk space errors
	Set proper file permissions
*/

bool	RequestProcessor::isCGI() {
	// TODO: Implement proper CGI detection logic
	// For now, return false to skip CGI processing
	return false;
}

bool	RequestProcessor::processCGI() {
	
	console::log("[INFO][POST REQUEST] Jim does a beautiful CGI process <3", MSG);
	_last_status = E_OK;
	return true;
}

bool	RequestProcessor::isFileUpload() {
	// TODO: Implement proper file upload detection logic
	// Check if any PostValue has is_file = true
	const std::map<std::string, PostValue>& post_data = _request->getPostData();
	for (std::map<std::string, PostValue>::const_iterator it = post_data.begin(); it != post_data.end(); ++it) {
		if (it->second.is_file)
			return true;
	}
	return false;
}

bool	RequestProcessor::processFileUpload() {
	console::log("[INFO][POST REQUEST] Processing file upload", MSG);
	// TODO: Implement actual file upload processing
	_last_status = E_OK;
	return true;
}

bool	RequestProcessor::checkUploadPermissions() {
	// TODO: Implement upload permission checking
	// Check if upload directory exists and is writable
	return true;
}

bool	RequestProcessor::processDeleteRequest() {
	console::log("[INFO][DELETE REQUEST] Processing DELETE request", MSG);
	// TODO: Implement DELETE request processing
	_last_status = E_OK;
	return true;
}