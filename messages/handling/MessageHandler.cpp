#include "MessageHandler.hpp"

void	handle_message(s_MessageStreams *streams) {

	// tmp
	// 	std::stringstream ss;
	// 	ss << streams.simple.rdbuf();
	// 	std::string	raw_request = ss.str();
	// 
	// 	HttpRequest request(raw_request);

	HttpRequest		request;
	HttpResponse	response;
	std::fstream	*stream;

	// 1. Decide which stream to read from
	if (streams->has_eof && !streams->is_chunked)
		stream = &streams->simple;
	else if (streams->is_chunked)
		stream = &streams->secondary;

	switch (request.getState()) {
		case s_msg_dead:
			break;
		case s_msg_empty:
			break;
		case s_msg_error:
			break;
		case s_msg_init:
			break;
		case s_msg_version:
			break;
		case s_req_start:
			break;
		case s_req_method:
			break;
		case s_req_uri:
			break;
		case s_req_done:
			break;
		case s_head_start:
			break;
		case s_head_done:
			break;
		case s_body_start:
			break;
		case s_body_content:
			break;
		case s_body_done:
			break;
		case s_msg_done:
			break;
	}
	
	std::string stream_line;

		stream_line = get_stream_line(streams->simple);
		stream_line = get_stream_line(streams->secondary);

		
	// with state checking and switch case
	// extract line
	// parse line
	// create request object
	// process request
	// create response
	// populate response stream

}
