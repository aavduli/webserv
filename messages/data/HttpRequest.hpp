#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include "HttpMessage.hpp"
#include "RequestUri.hpp"

class WebservConfig;

class RequestContext {
	
	private:
		std::string							_location_name;
		std::map<std::string, std::string>	_location_config;
		std::string							_document_root;
		bool								_autoindex_enabled;
		bool								_has_redirect;

	public:
		RequestContext();
		RequestContext(const WebservConfig& config);

		const std::string&		getLocationName() const;
		void					setLocationName(const std::string& name);
		const std::map<std::string, std::string>& getLocationConfig() const;
		void					setLocationConfig(const std::map<std::string, std::string>& loc_config);
		const std::string&		getDocumentRoot() const;
		void					setDocumentRoot(const std::string& root);
		bool					isAutoindexEnabled() const;
		void					setAutoindexEnabled(bool value);
		bool					hasRedirect() const;
		void					setRedirect(bool value);
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