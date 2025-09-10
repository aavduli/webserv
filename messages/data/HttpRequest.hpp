#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include "HttpMessage.hpp"

/* EXAMPLE USAGE FLOW:

// 1. PARSING PHASE
string raw_request = "GET /index.html HTTP/1.1\r\n...";
RequestParser parser;
HttpRequest* request = parser.parse(raw_request);

// 2. HANDLING PHASE  
RequestHandler handler;
HttpResponse* response = handler.process_request(request);

// 3. OUTPUT PHASE
ResponseHandler responder;
string response_string = responder.serialize(response);
 */

class HttpRequest : public HttpMessage {

	private:
		std::string	_method;
		std::string	_uri;		// needs to be a class?
		
	public:
		HttpRequest();
		HttpRequest(const HttpRequest& rhs);
		HttpRequest& operator=(const HttpRequest& rhs);
		~HttpRequest();

		std::string	getMethod() const;
		std::string	getUri() const;
};

void	print_request(HttpRequest request);

#endif // HTTPREQUEST_HPP