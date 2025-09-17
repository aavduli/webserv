#ifndef ONCONNECTION_HPP
# define ONCONNECTION_HPP

# include <string>
# include <map>

struct Conn {
	std::string in; //raw bytes
	bool header_done; //found \r\n\r\n
	bool chunked; //Transfer-encoding: chunked
	long content_len; //from content lenght (<= INT MAX ?)
	size_t body_have;
	size_t headers_end;

	Conn();
};

std::map<int, Conn> Conns;

enum { MAX_HEADER_BYTES = 16384}; //16kb

class onConn {
	public:
		onConn();
		~onConn();
	
		//if true, req_end receives the index *just past* the end of the first element
		static bool update_and_ready(Conn &c, size_t &req_end);

		//Utility if want to query header boundary externally
		static size_t headers_end_pos(const std::string &buf);

	private:
		static void try_mark_headers(Conn &c);
		static void inspect_headers_minimally(Conn &c); //sets content_len, chunked
		static void chunked_complete(const std::string &body, size_t &cut_after); //when chunked
};

#endif
