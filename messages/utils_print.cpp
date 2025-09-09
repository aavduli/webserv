#include "HttpRequest.hpp"

void	print_request(HttpRequest request) {
	std::cout << "\n=== HTTP REQUEST INFO ===" << std::endl;
	
	// Print HTTP version
	std::cout << "HTTP Version: " << request.getHttpVersion() << std::endl;
	
	// Print method and URI (HttpRequest specific)
	std::cout << "Method: " << request.getMethod() << std::endl;
	std::cout << "URI: " << request.getUri() << std::endl;
	
	// Print headers
	std::cout << "Headers:" << std::endl;
	HttpHeaders headers = request.getHeaders();
	std::cout << "  [Headers object created - specific values would need header parsing implementation]" << std::endl;
	
	// Print body
	std::cout << "Body: ";
	std::string body = request.getBody();
	if (body.empty()) {
		std::cout << "[Empty]" << std::endl;
	} else {
		std::cout << "\"" << body << "\"" << std::endl;
	}
	std::cout << "================================\n" << std::endl;
}