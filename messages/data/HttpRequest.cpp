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
std::map<std::string, PostData>	HttpRequest::getPostData() const {return _post_data;}
void				HttpRequest::setPostData(const std::map<std::string, PostData>& post_data) {_post_data = post_data;}

void	print_request(HttpRequest* request) {
	std::cout << "\n\n=== HTTP REQUEST ===" << std::endl;
	std::cout << "Method:		" << request->getMethod() << std::endl;
	std::cout << "URI:		" << request->getUri().getRawUri() << std::endl;
	std::cout << "HTTP Version:	" << request->getHttpVersionMajor() << "." << request->getHttpVersionMinor() << std::endl;
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