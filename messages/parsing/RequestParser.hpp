#ifndef REQUESTPARSER_HPP
#define REQUESTPARSER_HPP

#include "../handling/MessageHandler.hpp"

class RequestParser {

	private:
		HttpRequest*	_request;
		std::string		_raw_request;
		size_t			_current_pos;	// pos in raw_request string
		Status			_last_status;
		
		bool			parseRequestLine();
		bool			parseMethod(std::string request_line);
		bool			parseUri(std::string request_line);
		bool			parseVersion(std::string request_line);
		bool			parseHeaders();
		std::string		parseHeaderName(std::string line);
		bool			parseBody();

	public:
		RequestParser(HttpRequest* request, const std::string& raw_request);
		RequestParser(const RequestParser& rhs);
		RequestParser& operator=(const RequestParser& rhs);
		~RequestParser();
	
		bool	parseRequest();
		Status	getLastStatus() const;
};

#endif //REQUESTPARSER_HPP