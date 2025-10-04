#ifndef REQUESTPARSER_HPP
#define REQUESTPARSER_HPP

#include "MessageParser.hpp"
#include "../data/HttpRequest.hpp"

class RequestParser : public MessageParser {

	private:
		HttpRequest*	_request;

	public:
		RequestParser();
		RequestParser(const RequestParser& rhs);
		RequestParser& operator=(const RequestParser& rhs);
		~RequestParser();

		bool			parseRequest(HttpRequest* request, const std::string& raw_request);
		bool			parseRequestLine();
		bool			parseMethod(std::string request_line);
		bool			parseUri(std::string request_line);
		bool			parseVersion(std::string request_line);
		bool			parseHeaders();
		std::string		parseHeaderName(std::string line);
		bool			parseBody();
};

#endif //REQUESTPARSER_HPP