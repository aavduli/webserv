#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include "HttpMessage.hpp"

enum ResponseStatus {
	OK = 200
};

class HttpResponse : public HttpMessage {

	private:
		// std::map<int, std::string>	_status;	// status code (key) = reason phrase (value)
		// 										// OR
		// int			_status_code;				// code stored alone as int
		// std::string	_reason_phrase;				// short textual description associated to the status code
		// 										// OR
		ResponseStatus	_status;				// enum STR = int

	public:
		HttpResponse();
		HttpResponse(const HttpResponse& rhs);
		HttpResponse& operator=(const HttpResponse& rhs);
		~HttpResponse();

};

#endif // HTTPRESPONSE_HPP