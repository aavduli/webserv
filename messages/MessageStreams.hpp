#ifndef MESSAGESTREAMS_HPP
#define MESSAGESTREAMS_HPP

#include <iostream>
#include <fstream>

#include "../errors/errors.hpp"

struct s_MessageStreams {

	std::fstream&	simple;
	std::fstream&	secondary;	// chunked
	std::ostream&	response;
	bool			has_eof;
	bool			is_chunked;
	bool			response_done;
	
	// Constructor to initialize references
	s_MessageStreams(std::fstream& simple_stream, std::fstream& secondary_stream, std::ostream& response_stream)
		: simple(simple_stream), secondary(secondary_stream), response(response_stream),
		  has_eof(false), is_chunked(false), response_done(false) {}
};

#endif // MESSAGESTREAMS_HPP
