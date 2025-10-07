#include "ResponseGenerator.hpp"
#include "../../console/console.hpp"
#include <ctime>
#include <sstream>

ResponseGenerator::ResponseGenerator(const WebservConfig& config, HttpRequest* request, HttpResponse* response) : _config(config), _request(request), _response(response) {}
ResponseGenerator::ResponseGenerator(const ResponseGenerator& rhs) : _config(rhs._config), _request(rhs._request), _response(rhs._response) {}
ResponseGenerator& ResponseGenerator::operator=(const ResponseGenerator& rhs) {
	if (this != &rhs) {
		_request = rhs._request;
		_response = rhs._response;
		_last_status = rhs._last_status;
	}
	return *this;
}
ResponseGenerator::~ResponseGenerator() {}
void	ResponseGenerator::setLastStatus(Status last_status) {_last_status = last_status;}
Status	ResponseGenerator::getLastStatus() const {return _last_status;}

void ResponseGenerator::generateResponse() {

	// Set default headers for HTTP/1.1
	setDefaultHeaders();

	// Determine response type based on request and configuration
	console::log("[INFO] Last status: " + status_msg(_last_status), MSG);
	std::string path = _request->getUri().getEffectivePath();
	
	switch (_last_status) {
		case E_REDIRECT_PERMANENT:
		case E_REDIRECT_TEMPORARY: {
			generateRedirResponse();
			break;
		}
		case E_OK: {
			if (is_python_CGI(path))
				generateCGIResponse();
			else if (is_directory(path))
				generateDirectoryResponse();
			// else if (is_post)
			// else if (is_delete)
			else
				generateStaticFileResponse();
			break;
		}
		default: {
			generateErrorResponse();
			break;
		}
	}
}

std::string ResponseGenerator::serializeResponse() {

	if (!_response) {
		console::log("[ERROR] No response to serialize", MSG);
		return "";
	}
	
	// HTTP/1.1 status line
	// All headers with proper CRLF
	// Empty line
	// Body content

	// TODO: Implement response serialization
	// Format: HTTP/1.1 200 OK\r\nHeaders\r\n\r\nBody
	console::log("[INFO] Serializing response", MSG);
	return "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
}

void ResponseGenerator::generateStaticFileResponse() {

	console::log("[INFO] Generating static file response", MSG);
	
	const std::string& effective_path = _request->getUri().getEffectivePath();
	console::log("[INFO] Serving file: " + effective_path, MSG);

	// Content-Type header
	std::string extension = get_file_extension(effective_path);
	std::string content_type = get_mime_type(extension);
	_response->addHeader("Content-Type", str_to_vect(content_type, ""));

	// Opening file
	std::ifstream file(effective_path.c_str());
	if (!file.is_open()){
		console::log("[ERROR] Failed to read: " + effective_path, MSG);
		// _last_status = some permission error??
		return generateErrorResponse();
	}
	
	// Content-Length header
	std::string	str;
	std::string	file_contents;
	size_t		file_size = 0;
	while (std::getline(file, str)) {
		file_size += str.size();
		file_contents += str;
		file_contents.push_back('\n');
	}
	_response->addHeader("Content-Length", str_to_vect(nb_to_string(file_size), ""));
	_response->setStatus(E_OK);
}

void ResponseGenerator::generateDirectoryResponse() {

	console::log("[INFO] Generating directory listing response", MSG);

	// Check for index file first
	// If no index and autoindex enabled -> directory listing
		// Read directory contents
		// Generate HTML listing
		// Proper sorting and formatting
		// Handle permissions
	// If no index and autoindex disabled -> 403 Forbidden

	// TODO: Check if autoindex is enabled
	// TODO: Generate HTML directory listing
	// TODO: Set Content-Type to text/html
}

void ResponseGenerator::generateRedirResponse() {

	console::log("[INFO] Generating redirect response", MSG);
	
	const std::string& destination = _request->getUri().getRedirDestination();
	console::log("[INFO] Redirecting to: " + destination, MSG);
	
	// Parse location config "return" directive
	// Set 301/302 status
	// Set Location header
	// Optional redirect body

	// TODO: Set Location header
	// TODO: Set appropriate status code (301/302)
}

void ResponseGenerator::generateErrorResponse() {

	console::log("[INFO] Generating error response", MSG);
	
	Status status = getLastStatus();
	console::log("[INFO] Error status: " + status_msg(status), MSG);
	
	// Try custom error page from config
	// Fall back to default error page
	// Set appropriate headers
	// Set error status code

	// TODO: Generate appropriate error page
	// TODO: Set status code and error content
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

// std::string ResponseGenerator::generateDirectoryHTML(const std::string& directory_path) const {
// 	// TODO: Implement directory listing HTML generation
// 	console::log("[INFO] Generating directory HTML for: " + directory_path, MSG);
// 	
// 	std::stringstream html;
// 	html << "<!DOCTYPE html>\n";
// 	html << "<html><head><title>Directory Listing</title></head>\n";
// 	html << "<body><h1>Directory Listing</h1>\n";
// 	html << "<p>Directory: " << directory_path << "</p>\n";
// 	html << "<!-- TODO: Add actual directory contents -->\n";
// 	html << "</body></html>\n";
// 	
// 	return html.str();
// }
// 
// std::string ResponseGenerator::readFileContent(const std::string& file_path) const {
// 	// TODO: Implement file reading with proper error handling
// 	console::log("[INFO] Reading file content: " + file_path, MSG);
// 	
// 	// TODO: Open file, read content, handle errors
// 	return "<!-- File content placeholder -->";
// }

std::string get_mime_type(const std::string& extension) {

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
