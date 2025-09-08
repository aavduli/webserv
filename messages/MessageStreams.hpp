#ifndef MESSAGESTREAMS_HPP
#define MESSAGESTREAMS_HPP

#include <ostream>

struct s_msg_streams {

	std::ostream&	simple;
	std::ostream&	secondary;	// chunked
	std::ostream&	response;
	bool			has_eof;
	bool			is_chunked;
	bool			response_done;
	
	// Constructor to initialize references
	s_msg_streams(std::ostream& simple_stream, std::ostream& secondary_stream, std::ostream& response_stream)
		: simple(simple_stream), secondary(secondary_stream), response(response_stream),
		  has_eof(false), is_chunked(false), response_done(false) {}
};

#endif // MESSAGESTREAMS_HPP