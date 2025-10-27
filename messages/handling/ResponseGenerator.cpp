#include "ResponseGenerator.hpp"

ResponseGenerator::ResponseGenerator(const WebservConfig& config, HttpRequest* request, HttpResponse* response, Status status) : _config(config), _request(request), _response(response), _last_status(status), _done(false) {}
ResponseGenerator::ResponseGenerator(const ResponseGenerator& rhs) : _config(rhs._config), _request(rhs._request), _response(rhs._response), _last_status(rhs._last_status), _done(rhs._done) {}
ResponseGenerator& ResponseGenerator::operator=(const ResponseGenerator& rhs) {
	if (this != &rhs) {
		_request = rhs._request;
		_response = rhs._response;
		_last_status = rhs._last_status;
		_done = rhs._done;
	}
	return *this;
}
ResponseGenerator::~ResponseGenerator() {}
Status	ResponseGenerator::getLastStatus() const {return _last_status;}

void ResponseGenerator::generateResponse() {

	if (_last_status == E_REDIRECT_PERMANENT || _last_status == E_REDIRECT_TEMPORARY)
		generateRedirResponse();
	else if (_last_status == E_OK) {
		if (_request->getMethod() == "POST")
			generatePostResponse();
		else if (_request->getMethod() == "DELETE")
			generateDeleteResponse();
		else if (isValidCGI())
			generateCGIResponse();
		else {
			addValidIndex();
			if (is_directory(_request->getUri().getEffectivePath()))
				generateDirectoryResponse();
			else
				generateStaticFileResponse();
		}
	}
	else
		generateErrorResponse();
	setHeaders();
}

// TODO check if correct logic
void ResponseGenerator::generatePostResponse() {

	std::stringstream html;
	html << "<!DOCTYPE html>\n";
	html << "<html><head><title>Success</title></head>\n";
	html << "<body>\n";
	html << "<h1>Success!</h1>\n";
	html << "<p>Your POST request was processed successfully.</p>";

	// show post data if any
	const std::map<std::string, PostData>& post_data = _request->getPostData();
	if (!post_data.empty()) {
		html << "<p>";
		for (std::map<std::string, PostData>::const_iterator it = post_data.begin(); it != post_data.end(); ++it) {
			html << "<strong>" << it->first << ":</strong> ";
			if (it->second.is_file)
				html << it->second.new_filename;
			else
				html << it->second.content;
			html << "<br>";
		}
	}
	html << "<br><a href=\"/\">Back to homepage</a></p>\n";
	html << "</body></html>";

	_response->setStatus(E_OK);
	_response->setBody(html.str());
	_response->setBodyType(B_HTML);
}

// TODO check if correct logic
void ResponseGenerator::generateDeleteResponse() {

	const std::string& path = _request->getUri().getPath();
	std::stringstream html;
	html << "<!DOCTYPE html>\n";
	html << "<html><head><title>Success</title></head>\n";
	html << "<body>\n";
	html << "<h1>Success!</h1>\n";
	html << "<p>File <strong>" << path << "</strong> was successfully removed.</p>";
	html << "<br><a href=\"/\">Back to homepage</a></p>\n";
	html << "</body></html>";

	_response->setStatus(E_OK);
	_response->setBody(html.str());
	_response->setBodyType(B_HTML);
}

// non-blocking read with buffer
void ResponseGenerator::generateStaticFileResponse() {

	const std::string& path = _request->getUri().getEffectivePath();

	std::ifstream file(path.c_str());
	if (!file.is_open()) {
		console::log("[ERROR][GENERATE RESPONSE] Failed to open: " + path, MSG);
		_last_status = findErrorStatus(path);
		return generateErrorResponse();
	}
	_response->setBody(get_read_file_content(file));
	file.close();
	_response->setBodyType(B_FILE);
	_response->setStatus(E_OK);
	console::log("[INFO][GET] Static file				OK", MSG);
}

void ResponseGenerator::generateDirectoryResponse() {

	if (!_request->ctx._autoindex_enabled) {
		console::log("[ERROR][GENERATE RESPONSE] Autoindex off", MSG);
		_last_status = E_FORBIDDEN;
		return generateErrorResponse();
	}

	const std::string& path = _request->getUri().getEffectivePath();
	DIR* dir = opendir(path.c_str());
	if (!dir) {
		console::log("[ERROR][GENERATE RESPONSE] Couldn't open directory", MSG);
		_last_status = findErrorStatus(path);
		return generateErrorResponse();
	}
	_response->setBody(generateDirectoryHTML());
	_response->setBodyType(B_HTML);
	_response->setStatus(E_OK);
	closedir(dir);
	console::log("[INFO][GET] Directory listing			OK", MSG);
}

void ResponseGenerator::generateRedirResponse() {

	const std::string& destination = _request->getUri().getRedirDestination();
	_response->setStatus(_last_status);
	_response->addHeader("Location", str_to_vect(destination, ""));		// required
	_response->setBody(generateRedirHTML());
	_response->setBodyType(B_HTML);
	console::log("[INFO][GET] Redirection				OK", MSG);
}

void ResponseGenerator::generateErrorResponse() {

	_response->setStatus(_last_status);
	std::string error_page_path = _config.getErrorPage(_last_status);

	if (!error_page_path.empty()) {		// custom error page
		std::ifstream file(error_page_path.c_str());
		if (file.is_open()) {
			_response->setBody(get_read_file_content(file));
			file.close();
			_response->setBodyType(B_FILE);
			return ;
		}
		console::log("[ERROR][GENERATE RESPONSE] Failed to read custom error path: " + error_page_path, MSG);
	}
	_response->setBody(generateDefaultErrorHTML());
	_response->setBodyType(B_HTML);
	console::log("[INFO][RESPONSE] Error response		OK", MSG);
}

void ResponseGenerator::generateCGIResponse() {

	console::log("[INFO] Generating CGI response", MSG);

	const std::string& script_path = _request->getUri().getEffectivePath();
	std::string python_path = _config.getCgiPath(_request->ctx._location_name);

	CgiExec executor(script_path, python_path, &_config);
	std::string cgi_output = executor.execute(_request);

	if (cgi_output.empty()){
		console::log("[ERROR] CGI execution failed", MSG);
		_last_status = E_INTERNAL_SERVER_ERROR;
		return generateErrorResponse();
	}


	//parsing cgi output
	parseCGIOutput(cgi_output);
	_response->setStatus(E_OK);
	_response->setBodyType(B_CGI);  // CGI sets its own Content-Type
}

void ResponseGenerator::parseCGIOutput(const std::string& cgi_output){
	//first find the empty lime, separator header body
	size_t header_end= cgi_output.find("\n\n");
	if (header_end == std::string::npos){
		//no head ? :(
		_response->setBody(cgi_output);
		_response->setBodyType(B_CGI);
		return;
	}

	//separator header body
	std::string header_part = cgi_output.substr(0,header_end);
	std::string body_part = cgi_output.substr(header_end + 2); // count +2 for \n\n

	//parse header line by line
	std::istringstream header_stream(header_part);
	std::string line;
	while(std::getline(header_stream, line)){
		size_t colon_position = line.find(':');
		if (colon_position != std::string::npos){
			std::string header_name = line.substr(0, colon_position);
			std::string header_value = line.substr(colon_position + 1);

			//delet space
			while (!header_value.empty() && header_value[0] == ' '){
				header_value = header_value.substr(1);}

			_response->addHeader(header_name, str_to_vect(header_value, ""));
		}
	}
	_response->setBody(body_part);
}



void ResponseGenerator::setHeaders() {

	_response->addHeader("Date", str_to_vect(getCurrentGMTDate(), ""));
	_response->addHeader("Connection", str_to_vect("close", ""));

	if (_response->getBodyType() == B_FILE) {
		std::string extension = get_file_extension(_request->getUri().getEffectivePath());
		std::string content_type = getMimeType(extension);
		_response->addHeader("Content-Type", str_to_vect(content_type, ""));
	}
	else if (_response->getBodyType() == B_HTML)
		_response->addHeader("Content-Type", str_to_vect("text/html", ""));
	else if (_response->getBodyType() == B_JSON)
		_response->addHeader("Content-Type", str_to_vect("application/json", ""));
	else if (_response->getBodyType() == B_CGI)		// CGI scripts set their own Content-Type header, don't override
		console::log("[INFO] CGI response - Content-Type set by script", MSG);

	if (_response->getBodyType() == B_EMPTY)
		_response->addHeader("Content-Length", str_to_vect("0", ""));
	else {
		std::string body_size = nb_to_string(_response->getBody().size());
		_response->addHeader("Content-Length", str_to_vect(body_size, ""));
	}
}

std::string	ResponseGenerator::generateDirectoryHTML() {

	const std::string& url_path = _request->getUri().getPath();
	const std::string& dir_path = _request->getUri().getEffectivePath();
	DIR* dir = opendir(dir_path.c_str());
	if (!dir)
		return "<html><body><h1>Error: Cannot open directory</h1></body></html>";

	std::stringstream html;
	html << "<!DOCTYPE html>\n";
	html << "<html>\n<head>\n";
	html << "<title>Index of " << _request->ctx._location_name << "</title></head>\n";
	html << "<body><h1>Index of " << _request->ctx._location_name << "</h1>\n";

	if (url_path != "/" && url_path.length() > 1) {					// if not root, link parent dir
		std::string parent_path = remove_suffix(url_path, "/");		// remove trailing /
		size_t last_slash = parent_path.find_last_of('/');
		if (last_slash == 0)
			parent_path = "/";
		else
			parent_path = parent_path.substr(0, last_slash);
		html << "<p><a href=\"" << parent_path << "\">../</a>\n</p>";
	}

	struct dirent *en;
	while ((en = readdir(dir)) != NULL) {
		std::string name = en->d_name;
		if (name == "." || name == "..")
			continue;
		std::string file_path = build_full_path(url_path, name);
		html << "<p><a href=\"" << file_path << "\">" << name << "</a>\n</p>";
	}
	closedir(dir);
	html << "</body></html>";
	return html.str();
}

std::string	ResponseGenerator::generateDefaultErrorHTML() {

	std::stringstream html;
	html << "<!DOCTYPE html>\n";
	html << "<html><head><title>" << getLastStatus() << " - " << status_msg(_response->getStatus()) << "</title></head>\n";
	html << "<body><h1>" << getLastStatus() << " - " << status_msg(_response->getStatus()) << "</h1>\n";
	html << "<p>Very sad.</p>";
	html << "<p><a href=\"/\">Back to homepage</a></p>\n";
	html << "</body></html>";
	return html.str();
}

std::string	ResponseGenerator::generateRedirHTML() {

	std::stringstream html;
	html << "<!DOCTYPE html>\n";
	html << "<html><head><title>Redirected</title></head>\n";
	html << "<body>\n";
	html << "<h1>" << (_last_status == E_REDIRECT_PERMANENT ? "Moved Permanently" : "Moved Temporarily") << "</h1>\n";
	html << "<p>The document has moved <a href=\"" << _request->getUri().getRedirDestination() << "\">here</a>.</p>\n";
	html << "</body></html>";
	return html.str();
}

void	ResponseGenerator::addValidIndex() {

	std::string path = _request->getUri().getEffectivePath();
	if (!is_directory(path) || _request->ctx._index_list.empty())
		return ;

	const std::vector<std::string>& indexes = _request->ctx._index_list;
	std::vector<std::string>::const_iterator it;
	for (it = indexes.begin(); it != indexes.end(); it++) {

		std::string full_index_path = build_full_path(path, *it);
		if (is_valid_file_path(full_index_path)) {
			RequestUri uri(_request->getUri());
			uri.setEffectivePath(full_index_path);
			_request->setUri(uri);
			return ;
		}
	}
}

bool	ResponseGenerator::isValidCGI() const {

	std::string path = _request->getUri().getEffectivePath();
	if (path.empty() || !is_valid_file_path(path))
		return false;

	std::map<std::string, std::string> config = _request->ctx._location_config;
	std::string cgi_extensions = config["cgi_ext"];
	if (cgi_extensions.empty())
		return false;

	std::string extension = get_file_extension(path);
	if (!extension.empty() && extension[0] != '.')
		extension = "." + extension;

	std::vector<std::string> valid_cgi_extensions = str_to_vect(cgi_extensions, " ");
	for (size_t i = 0; i < valid_cgi_extensions.size(); i++) {
		if (extension == valid_cgi_extensions[i])
			return true;
	}
	return false;
}

Status	findErrorStatus(const std::string& path) {

	struct stat file_stat;

	// stat failed = invalid path
	if (stat(path.c_str(), &file_stat) != 0) {
		if (errno == ENOENT)
			return E_NOT_FOUND;					// 404 not found
		else if (errno == EACCES)
			return E_FORBIDDEN;					// 403 permission denied
		else
			return E_INTERNAL_SERVER_ERROR;		// 500 other error
	}

	// valid path
	if (S_ISREG(file_stat.st_mode)) {			// path valid, check
		if (!(file_stat.st_mode & S_IRUSR))		// file
			return E_FORBIDDEN; 				// 403 can't read
	}
	else if (S_ISDIR(file_stat.st_mode)) {		// directory
		if (!(file_stat.st_mode & S_IRUSR))		// autoindex on
			return E_FORBIDDEN;					// 403 can't read
	}
	return E_OK;
}

std::string getMimeType(const std::string& extension) {

	// Text types
	if (extension == "html" || extension == "htm") {
		return "text/html";
	}
	if (extension == "css") {
		return "text/css";
	}
	if (extension == "js") {
		return "application/javascript";
	}
	if (extension == "txt") {
		return "text/plain";
	}
	if (extension == "json") {
		return "application/json";
	}
	if (extension == "xml") {
		return "application/xml";
	}

	// Image types
	if (extension == "png") {
		return "image/png";
	}
	if (extension == "jpg" || extension == "jpeg") {
		return "image/jpeg";
	}
	if (extension == "gif") {
		return "image/gif";
	}
	if (extension == "svg") {
		return "image/svg+xml";
	}
	if (extension == "ico") {
		return "image/x-icon";
	}
	if (extension == "webp") {
		return "image/webp";
	}

	// Font types
	if (extension == "woff") {
		return "font/woff";
	}
	if (extension == "woff2") {
		return "font/woff2";
	}
	if (extension == "ttf") {
		return "font/ttf";
	}
	if (extension == "eot") {
		return "application/vnd.ms-fontobject";
	}

	// Document types
	if (extension == "pdf") {
		return "application/pdf";
	}
	if (extension == "doc") {
		return "application/msword";
	}
	if (extension == "docx") {
		return "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	}

	// Archive types
	if (extension == "zip") {
		return "application/zip";
	}
	if (extension == "tar") {
		return "application/x-tar";
	}
	if (extension == "gz") {
		return "application/gzip";
	}

	// Default for unknown extensions
	return "application/octet-stream";
}

std::string getCurrentGMTDate() {

	time_t now;
	struct tm* gmt;
	char buf[100];

	time(&now);				// get current time
	gmt = gmtime(&now);		// convert to GMT/UTC

	// %2d:%02d (gmt->tm_hour+2)%24, gmt->tm_min))

	// format: Sun, 06 Nov 1994 08:49:37 GMT
	strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M GMT", gmt);

	std::string date(buf);
	return date;
}
