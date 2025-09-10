#include "MessageParser.hpp"



// separate parsing in steps and monitor with state
// PARSING LOGIC
// 	
// 	if (raw_request.empty()) {
// 		_state = s_msg_empty;
// 		std::cerr << "raw_request empty" << std::endl;
// 		return false;
// 	}
// 
// 	_state = s_req_start;
// 	std::string::const_iterator it = raw_request.begin();
// 	std::string::const_iterator end = raw_request.end();
// 	
// 	while (it != end) {
// 		//std::cout << "current char [" << *it << "] state: " << _state << std::endl;
// 		switch (_state) {
// 			case s_req_start:
// 				if (!parse_req_method(it, end))
// 					return false;
// 				break;
// 			case s_req_method:
// 				if (!parse_req_uri(it, end))
// 					return false;
// 				break;
// 			case s_req_uri:
// 				if (!parse_req_version(it, end))
// 					return false;
// 				break;
// 			case s_req_version:
// 				if (!parse_req_headers(it, end))
// 					return false;
// 				break;
// 			case s_head_done:
// 				if (!parse_req_body(it, end))
// 					return false;
// 				break;
// 			case s_body_done:
// 				if (!is_valid_request())
// 					return false;
// 				break;
// 			case s_req_done:
// 				return true;
// 			default:
// 				std::cerr << "Unfinished/unknown parsing state" << std::endl;
// 				return false;
// 		}
// 	}

	// if it != end, chars remaining
	// Skip to end of line
	// while (it != end && *it != '\n')
	// 	++it;
	// Skip the \n
	// if (it != end && *it == '\n')
	// 	++it;

	// return (_state == s_req_done);


std::string	get_stream_line(std::fstream& stream) {
	// getline
	return "";
}




// TODO
bool RequestParser::parse_request_line() {
	return true;
}

// TODO: Implement header parsing
// For now, just skip to body or end
bool RequestParser::parse_headers() {
	std::cout << "Parsing headers (TODO)" << std::endl;
	_state = s_head_done;
	return true;
}

bool	RequestParser::parse_method() {

	// Skip LWS
// 	while (it != end && (*it == ' ' || *it == '\t'))
// 		++it;
// 
// 	std::string method;
// 	while (it != end && *it != ' ' && *it != '\t' && *it != '\r' && *it != '\n') {
// 		method += *it;
// 		++it;
// 	}
// 	if (method.empty()) {
// 		std::cerr << "No HTTP method found" << std::endl;
// 		return false;
// 	}
// 	else {
// 		_method = method;
// 		std::cout << "Method:		" << _method << std::endl;
// 		_state = s_req_method;
// 		return true;
// 	}
	return true;
}

bool	RequestParser::parse_uri() {

// 	// Skip SP between method and URI
// 	while (it != end && (*it == ' ' || *it == '\t'))
// 		++it;
// 
// 	std::string uri;
// 	while (it != end && *it != ' ' && *it != '\t' && *it != '\r' && *it != '\n') {
// 		uri += *it;
// 		++it;
// 	}
// 	if (uri.empty()) {
// 		std::cerr << "No URI found" << std::endl;
// 		return false;
// 	}
// 	else {
// 		_uri = uri;
// 		std::cout << "URI:		" << _uri << std::endl;
// 		_state = s_req_uri;
// 		return true;
	// }
	return true;
}

bool	RequestParser::parse_version() {

// 	// Skip SP between URI and http version
// 	while (it != end && (*it == ' ' || *it == '\t'))
// 		++it;
// 
// 	std::string version;
// 	while (it != end && *it != ' ' && *it != '\t' && *it != '\r' && *it != '\n') {
// 		version += *it;
// 		++it;
// 	}
// 	if (version.empty()) {
// 		std::cerr << "No HTTP version found" << std::endl;
// 		return false;
// 	}
// 	else {
// 
// 		std::cout << "HTTP version:	" << version << std::endl;
// 		_state = s_req_version;
// 		return true;
// 	}
	return true;
}

// TODO: Implement body parsing based on Content-Length or Transfer-Encoding
bool RequestParser::parse_body() {
	std::cout << "Parsing body (TODO)" << std::endl;
	_state = s_body_done;
	return true;
}