#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include "HttpMessage.hpp"

class HttpResponse : public HttpMessage {

	private:
		Status			_status;

	public:
		HttpResponse();
		HttpResponse(const HttpResponse& rhs);
		HttpResponse& operator=(const HttpResponse& rhs);
		~HttpResponse();

		Status			getStatus() const;
		void			setStatus(Status code);
};

#endif //HTTPRESPONSE_HPP