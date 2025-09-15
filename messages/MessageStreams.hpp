#ifndef MESSAGESTREAMS_HPP
#define MESSAGESTREAMS_HPP

#include <ostream>

struct s_msg_streams : public std::ostream {
	private:
	public:	
		s_msg_streams();
		std::ostream&	simple;
		std::ostream&	secondary;	// chunked
		std::ostream&	response;
		bool			has_eof;
		bool			is_chunked;
		bool			response_done;
	
};

#endif // MESSAGESTREAMS_HPP
