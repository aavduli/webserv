#include "HttpResponse.hpp"

HttpResponse::HttpResponse() : _status(E_INIT), _custom_msg("") {}

HttpResponse::HttpResponse(const HttpResponse& rhs) : HttpMessage(rhs), _status(rhs._status), _custom_msg(rhs._custom_msg) {}

HttpResponse& HttpResponse::operator=(const HttpResponse& rhs) {
	if (this != &rhs) {
		HttpMessage::operator=(rhs);
		_status = rhs._status;
		_custom_msg = rhs._custom_msg;
	}
	return *this;
}

HttpResponse::~HttpResponse() {}

Status	HttpResponse::getStatus() const {
	return _status;
}

void	HttpResponse::setStatus(Status status) {
	_status = status;
}

void	HttpResponse::setCustomMessage(const std::string& msg) {
	_custom_msg = msg;
}

std::string HttpResponse::getCustomMessage() const {
	return _custom_msg;
}
