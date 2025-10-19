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

	const std::vector<std::string>& ct_values = _request->getHeaderValues("Content-Type");
	if (ct_values.empty()) {
		console::log("[ERROR][POST BODY DECODING] Content-Type empty", MSG);
		_last_status = E_BAD_REQUEST;
	}

	std::string content_type = ct_values[0];
	if (content_type.find("application/x-www-form-urlencoded") != std::string::npos)
		processURLEncodedBody();
	else if (content_type.find("multipart/form-data") != std::string::npos)
		processMultipartBody();
	else if (content_type.find("text/plain") == std::string::npos) {
		console::log("[ERROR][POST BODY DECODING] Invalid Content-Type", MSG);
		_last_status = E_BAD_REQUEST;
	}
	if (_request->ctx._upload_enabled)
		processFileUpload();
	return (_last_status == E_OK);
}

void	RequestProcessor::processURLEncodedBody() {

	std::string raw_body = _request->getBody();
	std::map<std::string, PostData> data;

	std::vector<std::string> pairs = str_to_vect(_request->getBody(), "&");
	for (size_t i = 0; i < pairs.size(); i++) {
		if (pairs[i].empty())
			continue;

		size_t equal = pairs[i].find("=");
		if (equal == std::string::npos) {
			data[urlDecode(pairs[i])] = PostData("");
		}
		else {
			std::string key = urlDecode(pairs[i].substr(0, equal));
			std::string value = urlDecode(pairs[i].substr(equal + 1));
			data[key] = PostData(value);
		}
	}
	_request->setPostData(data);
	_last_status = E_OK;
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

void	RequestProcessor::processMultipartBody() {
	
	std::map<std::string, PostData> data;
	const std::string& raw_body = _request->getBody();
	std::string boundary = extractBoundary(_request->getHeaderValues("Content-Type"));
	
	if (boundary.empty()) {
		_last_status = E_BAD_REQUEST;
		return;
	}

	std::vector<std::string> parts = splitByBoundary(raw_body, boundary);
	if (parts.empty()) {
		_last_status = E_BAD_REQUEST;
		return;
	}

	for (size_t i = 0; i < parts.size(); i++) {
		if (!processMultipartPart(parts[i], data)) {
			_last_status = E_BAD_REQUEST;
			return;
		}
	}
	_request->setPostData(data);
	_last_status = E_OK;
}

bool	RequestProcessor::processMultipartPart(const std::string& part, std::map<std::string, PostData>& data) {
	
	size_t header_end = part.find("\r\n\r\n");
	if (header_end == std::string::npos) {
		console::log("[ERROR][MULTIPART] Missing header separator", MSG);
		return false;
	}
	
	std::map<std::string, std::vector<std::string> > headers = parseMultipartHeaders(part.substr(0, header_end));
	std::string field_name = extractDispositionData(headers, "name=\"");
	if (field_name.empty())
		return true; // skipping empty part
	
	PostData value;
	value.content = part.substr(header_end + 4);
	
	std::string filename = extractDispositionData(headers, "filename=\"");
	if (!filename.empty()) {
		value.filename = filename;
		value.is_file = true;
		if (headers.find("Content-Type") != headers.end() && !headers["Content-Type"].empty())
			value.content_type = headers["Content-Type"][0];
	}
	data[field_name] = value;
	return true;
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
		console::log("[ERROR][MULTIPART] Empty header section", MSG);
		return headers;
	}

	std::vector<std::string> lines = str_to_vect(header_str, "\r\n");
	for (size_t i = 0; i < lines.size(); i++) {
		if (lines[i].empty())
			continue ;

		std::string name;
		std::vector<std::string> values;
		if (::parseHeaderLine(lines[i], name, values)) {
			if (!values.empty())
				headers[name] = values;
		}
		else
			console::log("[ERROR][MULTIPART] Header section parsing failed", MSG);
	}
	return headers;
}

std::string RequestProcessor::extractDispositionData(const std::map<std::string, std::vector<std::string> >& headers, const std::string& key) {

	std::map<std::string, std::vector<std::string> >::const_iterator it = headers.find("Content-Disposition");
	if (it == headers.end() || it->second.empty()) {
		console::log("[ERROR][MULTIPART DISPOSITION] Content-Disposition header missingin section", MSG);
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
			console::log("[ERROR][MULTIPART DISPOSITION] Empty or malformed disposition attribute: " + key, MSG);
	}
	return value;
}

void	RequestProcessor::processFileUpload() {
	
	bool upload_config_done = false;
	const std::map<std::string, PostData>& post_data = _request->getPostData();
	std::map<std::string, PostData>::const_iterator	it;
	
	for (it = post_data.begin(); it != post_data.end(); ++it) {
		if (it->second.is_file) {
			if (!upload_config_done) {
				if (!configUploadDir())
					return;
				upload_config_done = true;
			}

			std::string upload_max = _request->ctx._location_config["client_max_body_size"];
			if (it->second.content.size() > to_size_t(upload_max)) {
				console::log("[ERROR][POST] File upload exceeds client_max_body_size: " + it->second.content.size(), MSG);
				_last_status = E_BAD_REQUEST;
				return;
			}

			const std::string& filename = generateFilename(it->second.filename);
			if (!writeFileToDisk(filename, it->second)) {
				console::log("[ERROR][POST] File upload failed: " + it->second.filename, MSG);
				return;
			}
		}
	}
	_last_status = E_OK;
}

bool	RequestProcessor::configUploadDir() {

	std::string dir_path = _request->ctx._upload_dir;

	if (_request->ctx._upload_enabled == false) {
		console::log("[ERROR][UPLOAD DIR] File upload not allowed", MSG);
		_last_status = E_METHOD_NOT_ALLOWED;
		return false;
	}
	if (!is_directory(dir_path)) {
		if (mkdir(dir_path.c_str(), 0755) == -1) {
			if (errno != EEXIST) {
				console::log("[ERROR][UPLOAD DIR] Upload directory creation failed: " + dir_path, MSG);
				_last_status = E_UNPROCESSABLE_CONTENT;
				return false;
			}
		}
	}
	if (access(dir_path.c_str(), W_OK) != 0) {
		console::log("[ERROR][UPLOAD DIR] Write not allowed on upload directory: " + dir_path, MSG);
		_last_status = E_UNPROCESSABLE_CONTENT;
		return false;
	}
	return true;
}

std::string	RequestProcessor::generateFilename(const std::string& og_name) {

	time_t now = time(0);
	std::stringstream o;
	std::string name;

	o << now << "_" << og_name;
	name = o.str();
	name = canonicalize_path(name);
	for (size_t i = 0; i < name.length(); i++) {
		if (name[i] == '/')
			name[i] = '_';
	}
	return name;
}

// TODO non-blocking
bool	RequestProcessor::writeFileToDisk(const std::string& filename, const PostData& file_data) {

	std::string full_path = build_full_path(_request->ctx._upload_dir, filename);

	// blocking operations
	std::ofstream file(full_path.c_str(), std::ios::binary);
	if (!file.is_open()) {
		console::log("[ERROR][WRITE UPLOAD FILE] UNable to open file", MSG);
		_last_status = E_UNPROCESSABLE_CONTENT;
		return false;
	}

	file.write(file_data.content.c_str(), file_data.content.size());
	file.close();
	_last_status = E_OK;
	return true;

	// Write file content using non-blocking write() calls
	// Handle partial writes (store state, resume on next write opportunity)
	// Handle disk space errors (ENOSPC)
	// Set appropriate file permissions after writing

	// _last_status = E_INSUFFICIENT_STORAGE; // Disk full
	// _last_status = E_UNPROCESSABLE_CONTENT; // Write error
	// _last_status = E_FORBIDDEN; // Permission error
	// _last_status = E_INTERNAL_SERVER_ERROR;
}

bool	RequestProcessor::processDeleteRequest() {
	console::log("[INFO][DELETE REQUEST] Processing DELETE request", MSG);

	// do something
	_last_status = E_OK;
	return true;
}