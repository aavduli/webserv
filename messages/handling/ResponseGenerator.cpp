#include "ResponseGenerator.hpp"
#include "../../console/console.hpp"
#include <ctime>
#include <sstream>

ResponseGenerator::ResponseGenerator(const WebservConfig& config, HttpRequest* request, HttpResponse* response) : _config(config), _request(request), _response(response), _done(false) {}
ResponseGenerator::ResponseGenerator(const ResponseGenerator& rhs) : _config(rhs._config), _request(rhs._request), _response(rhs._response), _done(rhs._done) {}
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
void	ResponseGenerator::setLastStatus(Status last_status) {_last_status = last_status;}
Status	ResponseGenerator::getLastStatus() const {return _last_status;}

void ResponseGenerator::generateResponse() {

	setDefaultHeaders();
	console::log("[INFO] Status at generateResponse start: " + status_msg(_last_status), MSG);

	if (_request->getMethod() == "GET") {
		addValidIndex();
		console::log("[INFO] Effective path: " + _request->getUri().getEffectivePath(), MSG);
		if (_last_status == E_REDIRECT_PERMANENT || _last_status == E_REDIRECT_TEMPORARY)
			generateRedirResponse();
		else if (is_directory(_request->getUri().getEffectivePath()))
			generateDirectoryResponse();	// need HTML generation
		else if (isValidCGI())
			generateCGIResponse();	// Jimmy
		else
			generateStaticFileResponse();
	}
	// else if (_request->getMethod() == "POST")
	// 	generatePostResponse();
	// else if (_request->getMethod() == "DELETE")
	// 	generateDeleteResponse();
	else
		generateErrorResponse();
	setContentHeaders();
}

void ResponseGenerator::generateStaticFileResponse() {

	const std::string& path = _request->getUri().getEffectivePath();
	console::log("[INFO] Serving file: " + path, MSG);
	
	std::ifstream file(path.c_str());
	if (!file.is_open()){
		console::log("[ERROR] Failed to read: " + path, MSG);
		_last_status = findErrorStatus(path);
		return generateErrorResponse();
	}
	_response->setBody(readFileContent(file));
	_response->setBodyType(B_FILE);
	_response->setStatus(E_OK);
}

void ResponseGenerator::generateDirectoryResponse() {
	console::log("[INFO] Generating directory listing response", MSG);
	
	if (!_request->ctx._autoindex_enabled) {
		console::log("[ERROR] Directory access forbidden", MSG);
		_last_status = E_FORBIDDEN;
		return generateErrorResponse();
	}

	const std::string& path = _request->getUri().getEffectivePath();
	DIR* dir = opendir(path.c_str());
	if (!dir) {
		console::log("[ERROR] Couldn't open directory", MSG);
		_last_status = findErrorStatus(path);
		return generateErrorResponse();
	}

	_response->setBody(generateDirectoryHTML(dir));
	_response->setBodyType(B_HTML);
	_response->setStatus(E_OK);
	closedir(dir);
}

void ResponseGenerator::generateRedirResponse() {

	console::log("[INFO] Generating redirect response", MSG);
	const std::string& destination = _request->getUri().getRedirDestination();
	
	_response->setStatus(_last_status);
	_response->addHeader("Location", str_to_vect(destination, ""));		// required
	_response->setBody(generateRedirHTML());
	_response->setBodyType(B_HTML);
}

void ResponseGenerator::generateErrorResponse() {

	console::log("[INFO] Generating error response with status: " + status_msg(getLastStatus()), MSG);

	_response->setStatus(_last_status);
	std::string error_page_path = _config.getErrorPage(_last_status);

	if (!error_page_path.empty()) {		// custom error page
		std::ifstream file(error_page_path.c_str());
		if (file.is_open()) {
			_response->setBody(readFileContent(file));
			_response->setBodyType(B_FILE);
			return ;
		}
		console::log("[ERROR] Failed to read custom error path: " + error_page_path, MSG);
	}
	_response->setBody(generateDefaultErrorHTML());
	_response->setBodyType(B_HTML);
}

void ResponseGenerator::generateCGIResponse() {

	console::log("[INFO] Generating CGI response", MSG);

	// Execute CGI script
	// Parse CGI output headers
	// Handle CGI errors
	// Set appropriate response

	// TODO: Execute CGI script
	// TODO: Parse CGI output
	// TODO: Set headers from CGI response
	_response->setBodyType(B_CGI);  // CGI sets its own Content-Type
}

void ResponseGenerator::setDefaultHeaders() {

	/*
	HTTP/1.1 Changes:
	Persistent connections are DEFAULT (keep-alive)
	Only close if client sends Connection: close or error occurs
	Must handle pipelined requests
	
	Accurate Content-Length for all responses
	Last-Modified headers for static files
	Server identification header
	Proper Date headers
	*/
	
	console::log("[INFO] Setting default HTTP/1.1 headers", MSG);
	
	// TODO: Set HTTP version to 1.1
	// TODO: Add required headers
	// setDateHeader();
	// setConnectionHeader();
}

void ResponseGenerator::setContentHeaders() {

	console::log("[INFO] Setting content headers", MSG);

	if (_response->getBodyType() == B_FILE) {
		std::string path = _request->getUri().getEffectivePath();
		std::string extension = get_file_extension(path);
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
	else
		_response->addHeader("Content-Length", str_to_vect(nb_to_string(_response->getBody().size()), ""));
}

// void ResponseGenerator::setConnectionHeader() {
// 	bool close_connection = shouldCloseConnection();
// 	console::log("[INFO] Connection will be " + std::string(close_connection ? "closed" : "kept alive"), MSG);
// 	
// 	// TODO: Set Connection header
// }
// 
// void ResponseGenerator::setContentHeaders(size_t content_length) {
// 	console::log("[INFO] Setting content headers, length: " + std::to_string(content_length), MSG);
// 	
// 	// TODO: Set Content-Length header
// 	// TODO: Set Content-Type header based on file or content
// }
// 
// void ResponseGenerator::setDateHeader() {
// 	std::string date = getCurrentHTTPDate();
// 	console::log("[INFO] Setting Date header: " + date, MSG);
// 	
// 	// TODO: Set Date header (required by HTTP/1.1)
// }

// bool ResponseGenerator::shouldCloseConnection() const {
// 	// Check if client requested connection close
// 	if (getRequest()->hasHeader("connection")) {
// 		const std::vector<std::string>& conn_headers = getRequest()->getHeaderValues("connection");
// 		for (size_t i = 0; i < conn_headers.size(); i++) {
// 			if (conn_headers[i] == "close") {
// 				return true;
// 			}
// 		}
// 	}
// 	
// 	// TODO: Check for error conditions that require connection close
// 	// TODO: Check server configuration
// 	
// 	// Default: keep-alive for HTTP/1.1
// 	return false;
// }
// 
// std::string ResponseGenerator::getCurrentHTTPDate() const {
// 	time_t now = time(0);
// 	struct tm* gmt = gmtime(&now);
// 	
// 	char buffer[100];
// 	strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", gmt);
// 	
// 	return std::string(buffer);
// }

std::string	ResponseGenerator::readFileContent(std::ifstream& file) const {
	
	std::string	str;
	std::string	file_contents;
	while (std::getline(file, str)) {
		file_contents += str;
		file_contents.push_back('\n');
	}
	return file_contents;
}

std::string	ResponseGenerator::generateDirectoryHTML(DIR *dir) {

	// 	Read directory contents
	// 	Generate HTML listing with links to folders
	// 	Proper sorting and formatting
	// 	Handle permissions

	// 	Serve a directory listing (HTML page showing files and subdirectories)
	// 	Generate an HTML response with clickable links to files/folders

	(void)dir;
	const std::string& directory_path = _request->getUri().getEffectivePath();
	
	// TODO: Implement directory listing HTML generation
	console::log("[INFO] Generating directory HTML for: " + directory_path, MSG);
	
	std::stringstream html;
	html << "<!DOCTYPE html>\n";
	html << "<html><head><title>Directory Listing</title></head>\n";
	html << "<body><h1>Directory Listing</h1>\n";
	html << "<p>Directory: " << directory_path << "</p>\n";
	html << "<!-- TODO: Add actual directory contents -->\n";
	html << "</body></html>\n";
	
	return html.str();
}

std::string	ResponseGenerator::generateDefaultErrorHTML() {

	console::log("[INFO] Generating default error HTML for status: " + _response->getStatus(), MSG);
	
	std::stringstream html;
	html << "<!DOCTYPE html>\n";
	html << "<html><head><title>Error</title></head>\n";
	html << "<body><h1>Error " << status_msg(_response->getStatus()) << "</h1>\n";
	html << "</body></html>\n";
	return html.str();
}

std::string	ResponseGenerator::generateRedirHTML() {

	console::log("[INFO] Generating redirection info HTML for status: " + _response->getStatus(), MSG);
	
	std::stringstream html;
	html << "<!DOCTYPE html>\n";
	html << "<html><head><title>Redirected</title></head>\n";
	html << "<body>\n";
	html << "<h1>" << (_last_status == E_REDIRECT_PERMANENT ? "Moved Permanently" : "Moved Temporarily") << "</h1>\n";
	html << "<p>The document has moved <a href=\"" << _request->getUri().getRedirDestination() << "\">here</a>.</p>\n";
	html << "</body></html>\n";
	return html.str();
}

void	ResponseGenerator::addValidIndex() {

	std::string path = _request->getUri().getEffectivePath();
	
	if (is_directory(path) && !_request->ctx._index_list.empty()) {
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

template<typename T>
std::string nb_to_string(T value) {
	std::ostringstream stream;
	stream << value;
	return stream.str();
}
