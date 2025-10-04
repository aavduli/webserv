#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include "HttpMessage.hpp"

 class HttpResponse : public HttpMessage {

	private:
		Status		_status;
		std::string	_custom_msg;

	public:
		HttpResponse();
		HttpResponse(const HttpResponse& rhs);
		HttpResponse& operator=(const HttpResponse& rhs);
		~HttpResponse();

		Status		getStatus() const;
		void		setStatus(Status code);
		void		setCustomMessage(const std::string& msg);
		std::string	getCustomMessage() const;

};

/* 
Headers :

If 405 Method Not Allowed:
	Content-Length: 0
	Date: Fri, 28 Jun 2024 14:30:31 GMT
	Server: ECLF (nyd/D179)
	Allow: GET, POST, HEAD
*/

#endif //HTTPRESPONSE_HPP