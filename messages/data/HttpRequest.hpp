#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include "HttpMessage.hpp"

class HttpRequest : public HttpMessage {

	private:
		HttpMethod	_method;
		std::string	_uri;		// needs to be a class?
		
	public:
		HttpRequest();
		HttpRequest(const HttpRequest& rhs);
		HttpRequest& operator=(const HttpRequest& rhs);
		~HttpRequest();

		HttpMethod	getMethod() const;
		void		setMethod(HttpMethod method);

		std::string	getUri() const;
		void		setUri(std::string uri);
};

#endif // HTTPREQUEST_HPP