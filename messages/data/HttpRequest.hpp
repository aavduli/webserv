#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include "HttpMessage.hpp"

/* 

<request-target>["?"<query>]

1.2 Extract and Validate URI
Parse the URI from request line
URL decode special characters (%20, etc.)
Validate against malicious paths (../, etc.)
Split path from query string
 */
struct s_request_uri {

	std::string		raw_uri;
	std::string		host;
	std::string		target;
	std::string		query;
	bool			is_valid;
	bool			is_absolute_path;
	bool			is_absolute_url;
};


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

void		print_request(HttpRequest* request);

#endif // HTTPREQUEST_HPP