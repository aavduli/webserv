#include "data/HttpRequest.hpp"
#include <iostream>

void	print_request(HttpRequest* request) {
	std::cout << "\n=== HTTP REQUEST ===" << std::endl;
	
	// Print request line
	std::cout << "Method:		" << request->getMethod() << std::endl;
	std::cout << "URI:		" << request->getUri() << std::endl;
	std::cout << "HTTP Version:	" << request->getHttpVersion() << std::endl;
	std::cout << "Headers:" << std::endl;
	request->printHeaders();
	std::cout << std::endl;
	std::string body = request->getBody();
	if (!body.empty()) {
		std::cout << "Body (" << body.length() << " bytes):" << std::endl;
		std::cout << body << std::endl;
	} else {
		std::cout << "Body: (empty)" << std::endl;
	}
	std::cout << "===================\n" << std::endl;
}