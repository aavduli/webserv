#ifndef HTTPMESSAGE_HPP
#define HTTPMESSAGE_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <map>
#include <vector>

#include "../MessageStreams.hpp"
#include "../parsing/MessageParser.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

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

class HttpRequest;
class HttpResponse;

class HttpMessage {

	protected:
		State			_state;			// monitor for parsing
		double			_http_version;	// <major>.<minor> format
		std::map<std::string, std::vector<std::string> >		_headers;		// general, request, response, entity header fields
		std::string		_body;			// optional
		
	public:
		HttpMessage();
		HttpMessage(const HttpMessage& rhs);
		HttpMessage& operator=(const HttpMessage& rhs);
		virtual ~HttpMessage();

		State		getState() const;
		void		setState(State state);
		double		getHttpVersion() const;
		void		setHttpVersion(double version);

		bool		hasHeader(const std::string& key) const;
		void		addHeader(const std::string& key, const std::string& value);
		std::vector<std::string>	getHeaderValues(const std::string& key) const;
		void		setHeaderValues(const std::string& key, const std::string& value);	// map?

		std::string	getBody() const;
		void		setBody(const std::string& body);
};

#endif // HTTPMESSAGE_HPP
