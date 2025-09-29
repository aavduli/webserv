#ifndef REQUESTPARSER_HPP
#define REQUESTPARSER_HPP

#include "MessageParser.hpp"
#include "../data/HttpRequest.hpp"

class RequestParser : public MessageParser {

	private:
		HttpRequest*	_request;		// for request parsing

	public:
		RequestParser(const WebservConfig& config);
		RequestParser(const RequestParser& rhs);
		RequestParser& operator=(const RequestParser& rhs);
		~RequestParser();

		HttpRequest*	parse_request(std::string raw_request);
		bool			parse_request_line();
		bool			parse_method(std::string request_line);
		bool			parse_uri(std::string request_line);
		bool			complete_uri();
		bool			parse_version(std::string request_line);
		bool			parse_headers();
		std::string		parse_header_name(std::string line);
		std::vector<std::string>	parse_header_values(std::string line);
		bool			parse_body();
};

#endif //REQUESTPARSER_HPP