#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include "HttpMessage.hpp"

enum StatusCode {
	OK = 200,
	CREATED = 201,
	BAD_REQUEST = 400,
	NOT_FOUND = 404,
	INTERNAL_SERVER_ERROR = 500
};

/* 
HTTP/1.1 200 OK
Content-Type: text/html
Content-Length: 456
Connection: keep-alive

[Body content]
 */

 class HttpResponse : public HttpMessage {

	private:
		int			_status_code;		// 3 digits
		std::string	_reason_phrase;		// short textual description

	public:
		HttpResponse();
		HttpResponse(const HttpResponse& rhs);
		HttpResponse& operator=(const HttpResponse& rhs);
		~HttpResponse();

		void		setStatus(StatusCode code);
		void		setCustomStatus(int code, const std::string& reason);
		std::string	getReasonPhrase() const;
};

#endif //HTTPRESPONSE_HPP