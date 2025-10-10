#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include "HttpMessage.hpp"
#include "RequestUri.hpp"

class WebservConfig;

struct RequestContext {
	
	public:
	std::string							_location_name;
	std::map<std::string, std::string>	_location_config;
	std::string							_document_root;
	std::vector<std::string>			_index_list;
	bool								_autoindex_enabled;
	bool								_has_redirect;
};

class HttpRequest : public HttpMessage {

	private:
		std::string		_method;
		RequestUri		_uri;
		
	public:
		RequestContext	ctx;
		
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