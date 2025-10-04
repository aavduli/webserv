#include "ResponseHandler.hpp"
#include "../../console/console.hpp"
#include <ctime>
#include <sstream>

ResponseHandler::ResponseHandler(HttpRequest* request, const WebservConfig& config) 
	: MessageHandler(request), _config(config) {}

ResponseHandler::ResponseHandler(const ResponseHandler& rhs) 
	: MessageHandler(rhs), _config(rhs._config) {}

ResponseHandler& ResponseHandler::operator=(const ResponseHandler& rhs) {
	if (this != &rhs) {
		MessageHandler::operator=(rhs);
	}
	return *this;
}

ResponseHandler::~ResponseHandler() {}

void ResponseHandler::generateResponse() {
	/*
	7. RESPONSE GENERATION:
	if (locationConfig["return"])           → Redirect response
	else if (isCGI(extension))             → CGI execution
	else if (isDirectory() && autoindex)   → Directory listing
	else                                   → Static file serving
	*/

	console::log("[INFO] Generating response", MSG);
	
	// Create response object if not exists
	if (!getResponse()) {
		// TODO: Initialize response object
		console::log("[INFO] Creating new response object", MSG);
	}
	
	// // Set default headers for HTTP/1.1
	// setDefaultHeaders();
	// 
	// // Determine response type based on request and configuration
	// Status status = getLastStatus();
	// 
	// switch (status) {
	// 	case E_REDIRECT_PERMANENT:
	// 	case E_REDIRECT_TEMPORARY:
	// 		generateRedirectResponse();
	// 		break;
	// 	case E_OK:
	// 		// Check if it's a directory, file, or CGI
	// 		generateStaticFileResponse();
	// 		break;
	// 	default:
	// 		generateErrorResponse();
	// 		break;
	// }
}
 
// std::string ResponseHandler::serializeResponse() {
// 	if (!getResponse()) {
// 		console::log("[ERROR] No response to serialize", MSG);
// 		return "";
// 	}
// 	
// 	// TODO: Implement response serialization
// 	// Format: HTTP/1.1 200 OK\r\nHeaders\r\n\r\nBody
// 	console::log("[INFO] Serializing response", MSG);
// 	return "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
// }
// 
// void ResponseHandler::generateStaticFileResponse() {
// 	console::log("[INFO] Generating static file response", MSG);
// 	
// 	const std::string& effective_path = getRequest()->getUri().getEffectivePath();
// 	console::log("[INFO] Serving file: " + effective_path, MSG);
// 	
// 	// TODO: Check if file exists, read content, set appropriate headers
// 	// TODO: Handle MIME types based on file extension
// 	// TODO: Set Content-Length header
// }
// 
// void ResponseHandler::generateDirectoryListingResponse() {
// 	console::log("[INFO] Generating directory listing response", MSG);
// 	
// 	// TODO: Check if autoindex is enabled
// 	// TODO: Generate HTML directory listing
// 	// TODO: Set Content-Type to text/html
// }
// 
// void ResponseHandler::generateRedirectResponse() {
// 	console::log("[INFO] Generating redirect response", MSG);
// 	
// 	const std::string& destination = getRequest()->getUri().getRedirDestination();
// 	console::log("[INFO] Redirecting to: " + destination, MSG);
// 	
// 	// TODO: Set Location header
// 	// TODO: Set appropriate status code (301/302)
// }
// 
// void ResponseHandler::generateErrorResponse() {
// 	console::log("[INFO] Generating error response", MSG);
// 	
// 	Status status = getLastStatus();
// 	console::log("[INFO] Error status: " + status_msg(status), MSG);
// 	
// 	// TODO: Generate appropriate error page
// 	// TODO: Set status code and error content
// }
// 
// void ResponseHandler::generateCGIResponse() {
// 	console::log("[INFO] Generating CGI response", MSG);
// 	
// 	// TODO: Execute CGI script
// 	// TODO: Parse CGI output
// 	// TODO: Set headers from CGI response
// }
// 
// void ResponseHandler::setDefaultHeaders() {
// 	/*
// 	HTTP/1.1 Changes:
// 	Persistent connections are DEFAULT (keep-alive)
// 	Only close if client sends Connection: close or error occurs
// 	Must handle pipelined requests
// 	*/
// 	
// 	console::log("[INFO] Setting default HTTP/1.1 headers", MSG);
// 	
// 	// TODO: Set HTTP version to 1.1
// 	// TODO: Add required headers
// 	setDateHeader();
// 	setConnectionHeader();
// }
// 
// void ResponseHandler::setConnectionHeader() {
// 	bool close_connection = shouldCloseConnection();
// 	console::log("[INFO] Connection will be " + std::string(close_connection ? "closed" : "kept alive"), MSG);
// 	
// 	// TODO: Set Connection header
// }
// 
// void ResponseHandler::setContentHeaders(size_t content_length) {
// 	console::log("[INFO] Setting content headers, length: " + std::to_string(content_length), MSG);
// 	
// 	// TODO: Set Content-Length header
// 	// TODO: Set Content-Type header based on file or content
// }
// 
// void ResponseHandler::setDateHeader() {
// 	std::string date = getCurrentHTTPDate();
// 	console::log("[INFO] Setting Date header: " + date, MSG);
// 	
// 	// TODO: Set Date header (required by HTTP/1.1)
// }
// 
// bool ResponseHandler::shouldCloseConnection() const {
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
// std::string ResponseHandler::getCurrentHTTPDate() const {
// 	time_t now = time(0);
// 	struct tm* gmt = gmtime(&now);
// 	
// 	char buffer[100];
// 	strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", gmt);
// 	
// 	return std::string(buffer);
// }
// 
// std::string ResponseHandler::getMimeType(const std::string& file_extension) const {
// 	// TODO: Implement MIME type detection based on file extension
// 	if (file_extension == "html" || file_extension == "htm") {
// 		return "text/html";
// 	}
// 	else if (file_extension == "css") {
// 		return "text/css";
// 	}
// 	else if (file_extension == "js") {
// 		return "application/javascript";
// 	}
// 	else if (file_extension == "png") {
// 		return "image/png";
// 	}
// 	else if (file_extension == "jpg" || file_extension == "jpeg") {
// 		return "image/jpeg";
// 	}
// 	
// 	return "application/octet-stream"; // Default binary type
// }
// 
// std::string ResponseHandler::generateDirectoryHTML(const std::string& directory_path) const {
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
// std::string ResponseHandler::readFileContent(const std::string& file_path) const {
// 	// TODO: Implement file reading with proper error handling
// 	console::log("[INFO] Reading file content: " + file_path, MSG);
// 	
// 	// TODO: Open file, read content, handle errors
// 	return "<!-- File content placeholder -->";
// }
