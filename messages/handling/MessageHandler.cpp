#include "MessageHandler.hpp"

MessageHandler::MessageHandler(HttpRequest* request) : _request(request), _response(NULL) {}

MessageHandler::MessageHandler(const MessageHandler& rhs) {
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

// bool validateHost(const HttpRequest& request, const ServerConfig& config) {
// 	std::string request_host = request.getHeader("host");
// 
// 	// Extract hostname and port from request
// 	std::string hostname, port;
// 	parseHostHeader(request_host, hostname, port);  // "localhost", "8080"
// 
// 	// Validate against config
// 	if (config.server_name != hostname) {
// 		return false;  // Could be 400 Bad Request or ignore
// 	}
// 
// 	if (config.listen_port != std::atoi(port.c_str())) {
// 		return false;  // Port mismatch
// 	}
// 
// 	return true;
// }

bool	MessageHandler::is_valid_request(const WebservConfig& config) const {

	// size_t max_body_size = to_size_t(_config.getDirective("max_body_size"));	// config use correct?
	// if (_content_length > max_body_size) {
	// 	console::log("\"Content-Length\" value > MAX_CONTENT_LENGTH", ERROR);
	// 	_state = s_req_invalid_content_length;
	// 	return 0;
	// }

	// check against server config

	// host header value <> server name and listen port

	// match routes
	// URI <> available server locations, if no match -> root or default index

	RequestUri uri(_request->getUri());
	if (uri.validate_with_config(config)) {
		std::cout << YELLOW;
		uri.print();
		std::cout << RESET << std::endl;
	}	
	return true;
}

void	MessageHandler::process_request() {
	
	if (!_request->getMethod().compare("GET")) {
		console::log("GET method", MSG);
		handle_get();
	}
	else if (!_request->getMethod().compare("POST")) {
		console::log("POST method", MSG);
		handle_post();
	}
	else if (!_request->getMethod().compare("DELETE")) {
		console::log("DELETE method", MSG);
		handle_delete();
	}
	else
		console::log("Unkown method", MSG);
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

// TODO set response status code and clean exit
void	MessageHandler::handle_get() {

	if (!(_request->getBody().empty())) {
		console::log("GET request shouldn't have a body", ERROR);
		_state = s_req_invalid_get;
		return ;
	}
	// if here, URI should not be empty
	std::string uri = _request->getUri().getRawUri();
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
		console::log("Empty request", MSG);
		return ;
	}

	// const char*		resp;
	RequestParser	parser;
	HttpRequest* request = parser.parse_request(raw);

	if (parser.getState() == s_req_parsing_done) {
		console::log("Request parsing success", MSG);
		
		MessageHandler handler(request);
		if (handler.is_valid_request(config)) {
			handler.process_request();
			handler.generate_response();
		}
		// resp = (handler.serialize_response()).c_str();
	}
	else
		std::cout << "[DEBUG] Request parsing failed with state " << parser.getState() << std::endl;
}
