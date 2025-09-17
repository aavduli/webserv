#include "MessageHandler.hpp"

MessageHandler::MessageHandler(HttpRequest* request) : _request(request), _response(NULL) {
	console::log("[MessageHandler Default Constructor]", DEBUG);
}

MessageHandler::MessageHandler(const MessageHandler& rhs) {
	console::log("[MessageHandler Copy Constructor]", DEBUG);
	if (rhs._request)
		_request = new HttpRequest(*rhs._request);
	else
		_request = NULL;
	if (rhs._response)
		_response = new HttpResponse(*rhs._response);
	else
		_response = NULL;
}

MessageHandler& MessageHandler::operator=(const MessageHandler& rhs) {
	console::log("[MessageHandler Assignement Operator]", DEBUG);
	if (this != &rhs) {
		if (rhs._request)
			_request = new HttpRequest(*rhs._request);
		else
			_request = NULL;
		if (rhs._response)
			_response = new HttpResponse(*rhs._response);
		else
			_response = NULL;
	}
	return *this;
}

MessageHandler::~MessageHandler() {
	console::log("[MessageHandler Destructor]", DEBUG);
	if (_request)
		delete _request;
	if (_response)
		delete _response;
}

HttpRequest*	MessageHandler::getRequest() const {
	return _request;
}

HttpResponse*	MessageHandler::getResponse() const {
	return _response;
}

bool	MessageHandler::is_valid_request() const {
	// check against server config
	return true;
}

void	MessageHandler::process_request() {
	print_request(_request);
	
	switch (_request->getMethod()) {
		case 0:	// GET
			console::log("GET method", INFO);
			handle_get();
			break;
		case 1:	// POST
			console::log("POST method", INFO);
			handle_post();
			break;
		case 2:	// DELETE
			console::log("DELETE method", INFO);
			handle_delete();
			break;
		case 3:	// HEAD
			console::log("HEAD method", INFO);
			handle_head();
			break;
		default:
			console::log("Unkown method", INFO);
			break;
	}
}

/* 
The GET HTTP method requests a representation of the specified resource. 

Example: GET <request-target>["?"<query>] HTTP/1.1

<request-target> identifies the target resource of the request when combined 
with the information provided in the Host header. 

Absolute path in requests to an origin server (e.g., /path/to/file.html) 
Absolute URL in requests to proxies (e.g., http://www.example.com/path/to/file.html)

<query> is an optional query component preceded by a question-mark ?.
Often used to carry identifying information in the form of key=value pairs.
*/

void	MessageHandler::handle_get() {

	if (!(_request->getBody().empty())) {
		console::log("GET request shouldn't have a body", ERROR);
		_state = s_req_invalid_get;
		// set response status code and clean exit
		return ;
	}
	// if here, URI should not be empty
	std::string uri = _request->getUri();
	std::cout << YELLOW << "[INFO] GET request URI: " << uri << RESET << std::endl;
}

void	MessageHandler::handle_post() {}

void	MessageHandler::handle_delete() {}

void	MessageHandler::handle_head() {}

// with default headers
void	MessageHandler::generate_response() {
	return ;
}

std::string	MessageHandler::serialize_response() {
	return "coucou";
}