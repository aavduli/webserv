#include "HttpResponse.hpp"

HttpResponse::HttpResponse() : _status(E_INIT) {}
HttpResponse::HttpResponse(const HttpResponse& rhs) : HttpMessage(rhs), _status(rhs._status) {}
HttpResponse& HttpResponse::operator=(const HttpResponse& rhs) {
	if (this != &rhs) {
		HttpMessage::operator=(rhs);
		_status = rhs._status;
	}
	return *this;
}
HttpResponse::~HttpResponse() {}

Status	HttpResponse::getStatus() const {return _status;}
void	HttpResponse::setStatus(Status status) {_status = status;}
