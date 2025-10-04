#include "RequestHandler.hpp"
#include "../../console/console.hpp"

RequestHandler::RequestHandler(HttpRequest* request) : MessageHandler(request) {}

RequestHandler::RequestHandler(const RequestHandler& rhs) : MessageHandler(rhs) {}

RequestHandler& RequestHandler::operator=(const RequestHandler& rhs) {
	if (this != &rhs) {
		MessageHandler::operator=(rhs);
	}
	return *this;
}

RequestHandler::~RequestHandler() {}

void RequestHandler::processRequest() {
	
	const std::string& method = getRequest()->getMethod();
	
	if (method == "GET") {
		console::log("[INFO] Processing GET method", MSG);
		processGetRequest();
	}
	else if (method == "POST" ) {
		console::log("[INFO] Processing POST method", MSG);
		processPostRequest();
	}
	else if (method == "DELETE") {
		console::log("[INFO] Processing DELETE method", MSG);
		processDeleteRequest();
	}
	else {
		console::log("[ERROR] Unknown method: " + method, MSG);
		setLastStatus(E_METHOD_NOT_ALLOWED);
	}
}

bool RequestHandler::validateGetRequest() {
	if (!getRequest()->getBody().empty()) {
		console::log("[ERROR] GET request shouldn't have a body", MSG);
		setLastStatus(E_BAD_REQUEST);
		return false;
	}
	return true;
}

bool RequestHandler::validatePostRequest() {
	// POST needs Content-Length or Transfer-Encoding header
	if (!getRequest()->hasHeader("content-length") || getRequest()->getHeaderValues("content-length").empty()) {
		if (getRequest()->getBodySize() == 0) {
			// Empty body is allowed for POST
			return true;
		}
		// 400 Bad Request if it cannot determine the length of the message
		if (!getRequest()->hasHeader("transfer-encoding") || getRequest()->getHeaderValues("transfer-encoding").empty()) {
			console::log("[ERROR] POST request missing Content-Length or Transfer-Encoding", MSG);
			//setLastStatus(E_LENGTH_REQUIRED);
			return false;
		}
	}
	return true;
}

bool RequestHandler::validateDeleteRequest() {
	// DELETE typically shouldn't have a body, but it's not forbidden
	return true;
}

void RequestHandler::processGetRequest() {

	if (!validateGetRequest()) {
		console::log("[ERROR] Invalid GET request", MSG);
		return ;
	}
	// Implementation for GET request processing
	console::log("[INFO] Processing GET request for URI: " + getRequest()->getUri().getPath(), MSG);
	// TODO: Implement GET logic (file serving, directory listing, etc.)
}

void RequestHandler::processPostRequest() {

	if (!validatePostRequest()) {
		console::log("[ERROR] Invalid GET request", MSG);
		return ;
	}
	// Implementation for POST request processing
	console::log("[INFO] Processing POST request for URI: " + getRequest()->getUri().getPath(), MSG);
	// TODO: Implement POST logic (form handling, file upload, CGI, etc.)
}

void RequestHandler::processDeleteRequest() {

	if (!validateDeleteRequest()) {
		console::log("[ERROR] Invalid GET request", MSG);
		return ;
	}
	// Implementation for DELETE request processing
	console::log("[INFO] Processing DELETE request for URI: " + getRequest()->getUri().getPath(), MSG);
	// TODO: Implement DELETE logic (file deletion, etc.)
}