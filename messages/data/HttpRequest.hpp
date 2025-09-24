#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include "HttpMessage.hpp"
#include "RequestUri.hpp"

class HttpRequest : public HttpMessage {

	private:
		HttpMethod		_method;
		RequestUri		_uri;
		
	public:
		HttpRequest();
		HttpRequest(const HttpRequest& rhs);
		HttpRequest& operator=(const HttpRequest& rhs);
		~HttpRequest();

		HttpMethod	getMethod() const;
		void		setMethod(HttpMethod method);

		RequestUri	getUri() const;
		void		setUri(const RequestUri& uri);
};

void		print_request(HttpRequest* request);

#endif // HTTPREQUEST_HPP