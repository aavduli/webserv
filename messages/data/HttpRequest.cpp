#include "HttpRequest.hpp"
#include "../../config/WebservConfig.hpp"

HttpRequest::HttpRequest() : _method(""), ctx() {}
HttpRequest::HttpRequest(const HttpRequest& rhs) : HttpMessage(rhs), _method(rhs._method), _uri(rhs._uri), ctx(rhs.ctx) {}
HttpRequest& HttpRequest::operator=(const HttpRequest& rhs) {
	if (this != &rhs) {
		HttpMessage::operator=(rhs);
		_method = rhs._method;
		_uri = rhs._uri;
		ctx = rhs.ctx;
	}
	return *this;
}
HttpRequest::~HttpRequest() {}

const std::string&	HttpRequest::getMethod() const {return _method;}
void				HttpRequest::setMethod(const std::string& method) {_method = method;}
RequestUri			HttpRequest::getUri() const {return _uri;}
void				HttpRequest::setUri(const RequestUri& uri) {_uri = uri;}

void	print_request(HttpRequest* request) {
	std::cout << "\n\n=== HTTP REQUEST ===" << std::endl;
	
	// Print request line
	std::cout << "Method:		" << request->getMethod() << std::endl;
	std::cout << "URI:		" << request->getUri().getRawUri() << std::endl;
	std::cout << "HTTP Version:	" << request->getHttpVersion() << std::endl;
	std::cout << "Headers:" << std::endl;
	request->printHeaders();
	std::string body = request->getBody();
	if (!body.empty()) {
		std::cout << "Body (" << body.length() << " bytes):" << std::endl;
		std::cout << body << std::endl;
	} else {
		std::cout << "Body: (empty)" << std::endl;
	}
	std::cout << "===================\n\n" << std::endl;
}

RequestContext::RequestContext() : _location_name(""), _location_config(), _document_root(""), _autoindex_enabled(false), _has_redirect(false) {}
RequestContext::RequestContext(const WebservConfig& config) : _location_name(""), _location_config(config.getServer()), _document_root(config.getRoot()), _autoindex_enabled(false), _has_redirect(false) {}
const std::string& RequestContext::getLocationName() const {return _location_name;}
void RequestContext::setLocationName(const std::string& name) {_location_name = name;}
const std::map<std::string, std::string>& RequestContext::getLocationConfig() const {return _location_config;}
void RequestContext::setLocationConfig(const std::map<std::string, std::string>& loc_config) {_location_config = loc_config;}
const std::string& RequestContext::getDocumentRoot() const {return _document_root;}
void RequestContext::setDocumentRoot(const std::string& root) {_document_root = root;}
const std::vector<std::string>&	RequestContext::getIndexList() const {return _index_list;}
void RequestContext::setIndexList(const std::vector<std::string>& indexes) {_index_list = indexes;}
bool RequestContext::isAutoindexEnabled() const {return _autoindex_enabled;}
void RequestContext::setAutoindexEnabled(bool value) {_autoindex_enabled = value;}
bool RequestContext::hasRedirect() const {return _has_redirect;}
void RequestContext::setRedirect(bool value) {_has_redirect = value;}