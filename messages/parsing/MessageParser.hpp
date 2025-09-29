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
#define MAX_URI_LENGTH 8000

#include "../../console/console.hpp"
#include "../../config/WebservConfig.hpp"
#include "../../errors/errors.hpp"
#include "../../parsing/Parsing.hpp"

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
	UNKNOWN		// 8
};

enum StatusCode {
	OK = 200,
	CREATED = 201,
	BAD_REQUEST = 400,
	NOT_FOUND = 404,
	INTERNAL_SERVER_ERROR = 500
};

enum State {
	s_msg_dead,
	s_msg_error,
	s_msg_init,
		
	/* REQUEST CHECKS */
	s_req_invalid_content_length,
	
	/* REQUEST PARSING */
	s_req_parsing_start, 
	s_req_parsing_headers,
	s_req_parsing_body,
	s_req_parsing_done, 

	/* REQUEST METHODS */
	s_req_invalid_get, 
	s_req_invalid_post, 
	s_req_invalid_delete, 
	s_req_invalid_head, 

	/* RESPONSE */
	s_res_start,
	s_res_protocol,
	s_res_version,
	s_res_status,
	s_res_reason,
	s_res_done,

	/* BODY */
	s_body_start,
	s_body_content,
	s_body_done,

	s_msg_done
};

class MessageParser {

	protected:
		Error			_error;
		State			_state;			// monitor for parsing
		std::string		_raw_data;		// raw_request
		size_t			_current_pos;	// pos in raw_data string

	public:
		MessageParser();
		MessageParser(const MessageParser& rhs);
		MessageParser& operator=(const MessageParser& rhs);
		virtual ~MessageParser();

		State	getState() const;
		void	setState(State state);
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
