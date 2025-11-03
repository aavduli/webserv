#ifndef ONCONNECTION_HPP
# define ONCONNECTION_HPP

# include <iostream>
# include <cstdlib>
# include <string>
# include <map>
# include <ctime>
# include "../console/console.hpp"

struct Conn {
	std::string in; //raw bytes
	bool header_done; //found \r\n\r\n
	bool chunked; //Transfer-encoding: chunked
	long content_len; //from content lenght (<= INT MAX ?)
	size_t body_have;
	size_t headers_end;
	time_t lastActivity;

	std::string outBuffer;
	size_t outSent;
	bool hasDataToSend;
	int serverFd;
	int clientPort;  // Track which port this connection came from

	bool waitingForCgi;

	Conn();
};


class onConn {
	public:
		onConn();
		~onConn();

	enum { MAX_HEADER_BYTES = 16384}; //16kb

		static bool update_and_ready(Conn &c, size_t &req_end);
		static bool onDiscon(Conn& c, bool alive, size_t endpos);
		static bool isTimedOut(Conn& c, time_t currentTime, int timeOutSecond);
		static void updateActivity(Conn& c);

		static size_t headers_end_pos(const std::string &buf);
		static std::string extractHostHeader(const Conn& c);

	private:
		static bool try_mark_headers(Conn &c);
		static void inspect_headers_minimally(Conn &c);
		static bool chunked_complete(const std::string &body, size_t &cut_after);
};

#endif
