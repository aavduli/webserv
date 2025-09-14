#ifndef MESSAGEPARSER_HPP
#define MESSAGEPARSER_HPP

#include <iostream>
#include <string>
#include <algorithm>
#include <map>
#include <vector>

#define CR	'\r'
#define LF	'\n'
#define MAX_CONTENT_LENGTH 1000000

#include "../../console/console.hpp"
#include "../../errors/errors.hpp"
#include "../../parsing/Parsing.hpp"
#include "../MessageStreams.hpp"

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

 class HttpMessage;
 class HttpRequest;
 class HttpResponse;

enum HttpMethod {
	GET,		// 0
	POST,		// 1
	DELETE,		// 2
	HEAD,		// 3
	PUT,		// 4
	CONNECT,	// 5
	OPTIONS,	// 6
	TRACE,		// 7
};

enum StatusCode {
	OK = 200,
	CREATED = 201,
	BAD_REQUEST = 400,
	NOT_FOUND = 404,
	INTERNAL_SERVER_ERROR = 500
};

enum State {
	s_msg_dead = 1,
	s_msg_error,
	s_msg_init,
		
	/* REQUEST CHECKS */
	s_req_incomplete,
	s_req_invalid_content_length,
	
	/* REQUEST PARSING */
	s_req_start, 
	s_req_line,
	s_req_done, 

	/* RESPONSE */
	s_res_start,
	s_res_protocol,
	s_res_version,
	s_res_status,
	s_res_reason,
	s_res_done,

	/* HEADERS PARSING */
	s_head_start,
	s_head_fields,
	s_head_done,

	/* BODY */
	s_body_start,
	s_body_content,
	s_body_done,

	s_msg_done
};

// PARENT CLASS = State manager, Line by line processing, Error handling
class MessageParser {

	protected:
		State			_state;			// monitor for parsing
		std::string		_raw_data;		// raw_request
		size_t			_current_pos;	// pos in raw_data string
		size_t			_content_length;

	public:
		MessageParser();
		MessageParser(const MessageParser& rhs);
		MessageParser& operator=(const MessageParser& rhs);
		virtual ~MessageParser();

		State	getState() const;
		void	setState(State state);

		size_t	getContentLength() const;

		bool	is_complete_request(const std::string& buffer);
		size_t	extract_content_length(const std::string& buffer);
};

// REQUEST PARSER (inherits from MessageParser)
class RequestParser : public MessageParser {

	private:
		HttpRequest*	_request;		// for request parsing

	public:
		RequestParser();
		RequestParser(const RequestParser& rhs);
		RequestParser& operator=(const RequestParser& rhs);
		~RequestParser();

		HttpRequest*	parse_request(std::string raw_request);
		bool			parse_request_line();
		bool			parse_method(std::string request_line);
		bool			parse_uri(std::string request_line);
		bool			parse_version(std::string request_line);
		bool			parse_headers();
		bool			parse_body();
};


// RESPONSE PARSER (inherits from MessageParser)
class ResponseParser : public MessageParser {

	private:
		HttpResponse*	_response;		// for response parsing

	public:
		ResponseParser();
		ResponseParser(const ResponseParser& rhs);
		ResponseParser& operator=(const ResponseParser& rhs);
		~ResponseParser();
};


#endif //MESSAGEPARSER_HPP
