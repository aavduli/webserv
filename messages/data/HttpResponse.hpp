#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

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

 class HttpResponse : public HttpMessage {

	private:
		int			_status_code;		// 3 digits
		std::string	_reason_phrase;		// short textual description

	public:
		HttpResponse();
		HttpResponse(const HttpResponse& rhs);
		HttpResponse& operator=(const HttpResponse& rhs);
		~HttpResponse();

		void		setStatus(StatusCode code);
		void		setCustomStatus(int code, const std::string& reason);
		std::string	getReasonPhrase() const;

};

#endif //HTTPRESPONSE_HPP