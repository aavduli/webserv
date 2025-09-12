#include "data/HttpRequest.hpp"
#include <iostream>

void	print_request(HttpRequest* request) {
	std::cout << "=== HTTP REQUEST ===" << std::endl;
	
	// Print request line
	std::cout << "Method:		" << request->getMethod() << std::endl;
	std::cout << "URI:		" << request->getUri() << std::endl;
	std::cout << "HTTP Version:	" << request->getHttpVersion() << std::endl;
	
	std::cout << std::endl;
	
	// Print headers
	std::cout << "Headers:" << std::endl;
	// Note: We'll need to iterate through headers, but the current interface
	// doesn't provide a way to get all headers. For now, we'll print common ones.
	
	// Try to print common headers if they exist
	try {
		std::vector<std::string> hostValues = request->getHeaderValues("Host");
		if (!hostValues.empty()) {
			std::cout << "  Host: " << hostValues[0] << std::endl;
		}
		
		std::vector<std::string> userAgentValues = request->getHeaderValues("User-Agent");
		if (!userAgentValues.empty()) {
			std::cout << "  User-Agent: " << userAgentValues[0] << std::endl;
		}
		
		std::vector<std::string> contentLengthValues = request->getHeaderValues("Content-Length");
		if (!contentLengthValues.empty()) {
			std::cout << "  Content-Length: " << contentLengthValues[0] << std::endl;
		}
		
		std::vector<std::string> contentTypeValues = request->getHeaderValues("Content-Type");
		if (!contentTypeValues.empty()) {
			std::cout << "  Content-Type: " << contentTypeValues[0] << std::endl;
		}
	} catch (...) {
		std::cout << "  (Error reading headers)" << std::endl;
	}
	
	std::cout << std::endl;
	
	// Print body
	std::string body = request->getBody();
	if (!body.empty()) {
		std::cout << "Body (" << body.length() << " bytes):" << std::endl;
		std::cout << body << std::endl;
	} else {
		std::cout << "Body: (empty)" << std::endl;
	}
	
	std::cout << "===================" << std::endl;
}