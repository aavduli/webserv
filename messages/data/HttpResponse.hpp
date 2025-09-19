#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include "HttpMessage.hpp"

 class HttpResponse : public HttpMessage {

	private:
		int			_status_code;
		std::string	_reason_phrase;

	public:
		HttpResponse();
		HttpResponse(const HttpResponse& rhs);
		HttpResponse& operator=(const HttpResponse& rhs);
		~HttpResponse();

		int			getStatus() const;
		void		setStatus(StatusCode code);
		void		setCustomStatus(int code, const std::string& reason);
		std::string	getReasonPhrase() const;

};

#endif //HTTPRESPONSE_HPP