#ifndef MESSAGEHANDLER_HPP
#define MESSAGEHANDLER_HPP

#include "../parsing/MessageParser.hpp"
#include "../MessageStreams.hpp"
#include "../data/HttpMessage.hpp"

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

 class MessageHandler {

	public:

		// from request data
		bool	is_valid_request();
		void	process_request();
		void	generate_response();

		// from response data
		void	format_response();	// string?
		void	add_headers();		// default headers?
		void	encode_response();	// serialization
};

void	handle_message(s_MessageStreams *streams);

#endif //MESSAGEHANDLER_HPP