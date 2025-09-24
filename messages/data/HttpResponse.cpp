#include "HttpResponse.hpp"

HttpResponse::HttpResponse() : _status_code(0), _reason_phrase("") {}

HttpResponse::HttpResponse(const HttpResponse& rhs) : HttpMessage(rhs), _status_code(rhs._status_code), _reason_phrase(rhs._reason_phrase) {}

HttpResponse& HttpResponse::operator=(const HttpResponse& rhs) {
	if (this != &rhs) {
		HttpMessage::operator=(rhs);
		_status_code = rhs._status_code;
		_reason_phrase = rhs._reason_phrase;
	}
	return *this;
}

HttpResponse::~HttpResponse() {}

int	HttpResponse::getStatus() const {
	return _status_code;
}

void	HttpResponse::setStatus(StatusCode code) {
	_status_code = static_cast<int>(code);
	_reason_phrase = getReasonPhrase();
}

void	HttpResponse::setCustomStatus(int code, const std::string& reason) {
	_status_code = code;
	if (reason.empty())
		_reason_phrase = getReasonPhrase();
	else
		_reason_phrase = reason;
}

std::string HttpResponse::getReasonPhrase() const {
	switch (_status_code) {
		case 200: return "OK";
		case 201: return "Created";
		case 400: return "Bad Request";
		case 404: return "Not Found";
		case 500: return "Internal Server Error";
		default: return "Unknown";
	}
}
