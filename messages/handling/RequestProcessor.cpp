#include "RequestProcessor.hpp"
#include <cerrno>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>

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
		console::log("[ERROR][POST] Content-Type empty", MSG);
		_last_status = E_BAD_REQUEST;
	}

	std::string content_type = ct_values[0];
	if (content_type.find("application/x-www-form-urlencoded") != std::string::npos)
		processURLEncodedBody();
	else if (content_type.find("multipart/form-data") != std::string::npos)
		processMultipartBody();
	else if (content_type.find("text/plain") == std::string::npos) {
		console::log("[ERROR][POST] Invalid Content-Type", MSG);
		_last_status = E_UNSUPPORTED_MEDIA_TYPE;
	}
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
	
	// look for method override
	if (data.find("_method") != data.end() && data["_method"].content == "DELETE") {
		_request->setMethod("DELETE");
		console::log("[INFO][DELETE] Method override detected: DELETE", MSG);
		processDeleteRequest();
		return;
	}
	_last_status = E_OK;
	console::log("[INFO][POST] URL-encoded body			OK", MSG);

}

void	RequestProcessor::processMultipartBody() {
	
	std::map<std::string, PostData> data;
	const std::string& raw_body = _request->getBody();

	std::string boundary = extractBoundary(_request->getHeaderValues("Content-Type"));
	std::vector<std::string> parts = splitByBoundary(raw_body, boundary);

	if (parts.empty()) {
		_last_status = E_BAD_REQUEST;
		return;
	}

	for (size_t i = 0; i < parts.size(); i++) {
		if (parts[i].find(boundary + "--") != std::string::npos)
			break;
		if (!processMultipartPart(parts[i], data, boundary)) {
			_last_status = E_BAD_REQUEST;
			return;
		}
	}
	_request->setPostData(data);
	_last_status = E_OK;
	console::log("[INFO][POST] Multipart body			OK", MSG);
}

bool	RequestProcessor::processMultipartPart(std::string& part, std::map<std::string, PostData>& data, const std::string& boundary) {
	
	size_t header_end = part.find("\r\n\r\n");
	if (header_end == std::string::npos) {
		console::log("[ERROR][MULTIPART] Missing header separator", MSG);
		return false;
	}

	std::string header_str = part.substr(0, header_end);
	std::map<std::string, std::vector<std::string> > headers = parseMultipartHeaders(header_str, boundary);
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
		if (!processFileUpload(value))
			return false;
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
	
	if (raw_parts.size() < 2) {
		console::log("[DEBUG][MULTIPART] Not enough parts found", MSG);
		return parts;
	}

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

std::map<std::string, std::vector<std::string> > RequestProcessor::parseMultipartHeaders(std::string& header_str, const std::string& boundary) {

	size_t start = header_str.find(boundary);
	header_str = header_str.substr(start + boundary.length());
	
	std::map<std::string, std::vector<std::string> >	headers;
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
		console::log("[ERROR][MULTIPART DISPOSITION] Content-Disposition header missing in section", MSG);
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

bool	RequestProcessor::processFileUpload(PostData& data) {

	if (_request->ctx._upload_enabled == false) {
		console::log("[ERROR][UPLOAD DIR] File upload not allowed", MSG);
		_last_status = E_METHOD_NOT_ALLOWED;
		return false;
	}

	std::string dir_path = _request->ctx._upload_dir;
	if (is_accessible_path(dir_path)) {
		if (access(dir_path.c_str(), W_OK) != 0) {
			console::log("[ERROR][UPLOAD DIR] Permission denied", MSG);
			_last_status = E_FORBIDDEN;
			return false;
		}
	}
	else if (mkdir(dir_path.c_str(), 0755) == -1) {
		console::log("[ERROR][UPLOAD DIR] Failed to create directory " + dir_path, MSG);
		_last_status = E_UNPROCESSABLE_CONTENT;
		return false;
	}

	data.new_filename = generateFilename(data.filename, _request->ctx._upload_dir);
	if (!writeFileUpload(data))
		return false;
	console::log("[INFO][POST] File upload				OK", MSG);
	_last_status = E_OK;
	return true;
}

bool	RequestProcessor::writeFileUpload(PostData& file_data) {

	std::string filename = file_data.new_filename;
	if (filename.empty()) {
		_last_status = E_BAD_REQUEST;
		return false;
	}

	std::string full_path = build_full_path(_request->ctx._upload_dir, filename);
	std::ofstream file(full_path.c_str(), std::ios::binary);
	if (!file.is_open()) {
		console::log("[ERROR][WRITE FILE] Unable to open file " + filename, MSG);
		_last_status = findErrorStatus(full_path);
		return false;
	}
	file.write(file_data.content.c_str(), file_data.content.size());
	file.close();
	_last_status = E_OK;
	return true;
}

bool	RequestProcessor::processDeleteRequest() {

	std::string target_path;

	const std::map<std::string, PostData>& post_data = _request->getPostData();
	if (!post_data.empty() && post_data.find("filename") != post_data.end()) {
		std::string filename = post_data.at("filename").content;
		if (filename.empty()) {
			console::log("[ERROR][DELETE] No filename provided in form", MSG);
			_last_status = E_BAD_REQUEST;
			return false;
		}
		target_path = _request->ctx._upload_dir + "/" + filename;
	}
	else
		target_path = _request->getUri().getEffectivePath();
// 
// 	if (target_path.find("files") == std::string::npos) {
// 		console::log("[ERROR][DELETE] Forbidden: File must be in files directory", MSG);
// 		_last_status = E_FORBIDDEN;
// 		return false;
// 	}
	if (!is_within_root(target_path, _request->ctx._document_root)) {
		console::log("[ERROR][VALIDATION] Path escapes document root", MSG);
		_last_status = E_FORBIDDEN;
		return false;
	}

	if (!is_valid_file_path(target_path)) {
		console::log("[ERROR][DELETE] File not found: " + target_path, MSG);
		_last_status = E_NOT_FOUND;
		return false;
	}

	if (unlink(target_path.c_str()) != 0) {
		console::log("[ERROR][DELETE] Failed to delete file: " + target_path, MSG);
		_last_status = E_INTERNAL_SERVER_ERROR;
		return false;
	}
	console::log("[INFO][DELETE] File deleted			OK", MSG);
	_last_status = E_OK;
	return true;
}

ssize_t	write_on_fd(const int fd, const std::string& content, size_t& pos, size_t buf_size) {

	ssize_t	bytes_written = 0;
	while (pos < content.size()) {
		bytes_written = write(fd, &content[pos], buf_size);
		if (bytes_written == -1)
			return bytes_written;
		pos += bytes_written;
	}
	return bytes_written;
}

std::string	generateFilename(const std::string& wanted_name, const std::string& upload_dir) {

	std::string filename;
	if (wanted_name.empty())
		filename = "file";
	else if (contains_unsafe_chars(wanted_name)) {
		console::log("[ERROR][UPLOAD FILE] Unsafe file name: " + wanted_name, MSG);
		return "";
	}
	else
		filename = wanted_name;
	filename = canonicalize_path(filename);

	std::string full_name = build_full_path(upload_dir, filename);
	if (!is_accessible_path(full_name))
		return filename;
	
	std::string extension = get_file_extension(filename);
	std::string base_name = filename.substr(0, filename.size() - (extension.size() + 1));
	std::string unique_name;

	int counter = 1;
	for (; counter < MAX_FILENAME_COUNTER; counter++) {
		unique_name = base_name + nb_to_string(counter);
		if (!extension.empty())
			unique_name = unique_name + "." + extension;
		full_name = build_full_path(upload_dir, unique_name);
		if (!is_accessible_path(full_name))
			break;
	}
	if (counter == MAX_FILENAME_COUNTER) {
		console::log("[ERROR][UPLOAD FILE] More than 9999 files with same name...", MSG);
		return "";
	}
	return unique_name;
}
