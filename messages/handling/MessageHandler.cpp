#include "MessageHandler.hpp"

MessageHandler::MessageHandler(const WebservConfig& config, HttpRequest* request) : _config(config), _request(request), _response(NULL) {
	console::log("[MessageHandler Default Constructor]", INFO, AH);
}

MessageHandler::MessageHandler(const MessageHandler& rhs) : _config(rhs._config) {
	console::log("[MessageHandler Copy Constructor]", INFO, AH);
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
	console::log("[MessageHandler Assignement Operator]", INFO, AH);
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
	console::log("[MessageHandler Destructor]", INFO, AH);
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
	
	switch (_request->getMethod()) {
		case 0:
			console::log("GET method", INFO, AH);
			handle_get();
			break;
		case 1:
			console::log("POST method", INFO, AH);
			handle_post();
			break;
		case 2:
			console::log("DELETE method", INFO, AH);
			handle_delete();
			break;
		case 3:
			console::log("HEAD method", INFO, AH);
			handle_head();
			break;
		default:
			console::log("Unkown method", INFO, AH);
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
		console::log("GET request shouldn't have a body", ERROR, ALL);
		_state = s_req_invalid_get;
		// TODO set response status code and clean exit
		return ;
	}
	// if here, URI should not be empty
	std::string uri = _request->getUri().getRawUri();
	std::cout << YELLOW << "[INFO] URI: " << uri << RESET << std::endl;
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

void	handle_request(const WebservConfig& config, const std::string &raw) {

	if (raw.empty()) {
		// return status code? return error/bool?
		console::log("Empty request", WARNING, AH);
		return ;
	}

	const char*		resp;
	RequestParser	parser(config);

	if (parser.is_complete_request(raw)) {

		HttpRequest* request = parser.parse_request(raw);
		if (parser.getState() == s_msg_done) {
			console::log("Request parsing success", INFO, AH);
			MessageHandler handler(config, request);
			if (handler.is_valid_request()) {
				handler.process_request();
				handler.generate_response();
			}
			resp = (handler.serialize_response()).c_str();
			std::cout << "RESPONSE: " << resp << std::endl;
		}
		else
			console::log("Request parsing failed", ERROR, ALL);
		// delete request;
	}
	else
		console::log("Incomplete request", ERROR, ALL);
}
