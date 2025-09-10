#ifndef MESSAGEPARSER_HPP
#define MESSAGEPARSER_HPP

#include <iostream>
#include <string>
#include <algorithm>
#include <map>
#include <vector>

#define CR	'\r'
#define LF	'\n'

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
	s_msg_empty,
	s_msg_error,
	s_msg_init,
	s_msg_version,

	s_line_processing,
	s_line_complete,
	s_line_unexpected_end,
	s_line_empty,

	/* REQUEST */
	s_req_start, 
	s_req_method, 
	s_req_uri, 
	s_req_version, 
	s_req_done, 

	/* RESPONSE */
	s_res_start,
	s_res_protocol,
	s_res_version,
	s_res_status,
	s_res_reason,
	s_res_done,

	/* HEADERS */
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
		std::string		_line;			// line being parsed
		std::string*	_line_ptr;		// ptr to char in current line
		int				_error_status;	// relevant?

	public:
		MessageParser();
		MessageParser(const MessageParser& rhs);
		MessageParser& operator=(const MessageParser& rhs);
		virtual ~MessageParser();

		// ACCESSORS?
};

std::string	get_stream_line(std::fstream& stream);


// REQUEST PARSER (inherits from MessageParser)
class RequestParser : public MessageParser {

	private:

	public:
		RequestParser();
		RequestParser(const RequestParser& rhs);
		RequestParser& operator=(const RequestParser& rhs);
		~RequestParser();

		bool	parse_request_line();
		bool	parse_headers();
		bool	parse_method();
		bool	parse_uri();
		bool	parse_version();
		bool	parse_body();
};


// RESPONSE PARSER (inherits from MessageParser)
class ResponseParser : public MessageParser {

	private:

	public:
		ResponseParser();
		ResponseParser(const ResponseParser& rhs);
		ResponseParser& operator=(const ResponseParser& rhs);
		~ResponseParser();
};

#endif //MESSAGEPARSER_HPP
