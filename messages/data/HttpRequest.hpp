#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include "HttpMessage.hpp"
#include "RequestUri.hpp"

// struct RequestContext {
// 	HttpRequest request;
// 	std::map<std::string, std::string> location_config;
// 	std::string location_prefix;
// 	std::string resolved_path;
// 	Status status;
// };

class HttpRequest : public HttpMessage {

	private:
		std::string		_method;
		RequestUri		_uri;

	public:
		HttpRequest();
		HttpRequest(const HttpRequest& rhs);
		HttpRequest& operator=(const HttpRequest& rhs);
		~HttpRequest();

		const std::string&	getMethod() const;
		void				setMethod(const std::string& method);

		RequestUri			getUri() const;
		void				setUri(const RequestUri& uri);
};

void		print_request(HttpRequest* request);

#endif // HTTPREQUEST_HPP