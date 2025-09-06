#include "HttpResponse.hpp"
#include <iostream>

HttpResponse::HttpResponse() {
	std::cout << "[HttpResponse Default Constructor]" << std::endl;
}

HttpResponse::HttpResponse(const HttpResponse& rhs) : HttpMessage(rhs), _status(rhs._status) {
	std::cout << "[HttpResponse Copy Constructor]" << std::endl;
}

HttpResponse& HttpResponse::operator=(const HttpResponse& rhs) {
	std::cout << "[HttpResponse Assignment Operator]" << std::endl;
	if (this != &rhs) {
		HttpMessage::operator=(rhs);
		_status = rhs._status;
	}
	return *this;
}

HttpResponse::~HttpResponse() {
	std::cout << "[HttpResponse Destructor]" << std::endl;
}