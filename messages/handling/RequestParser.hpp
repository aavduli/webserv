#ifndef REQUESTPARSER_HPP
#define REQUESTPARSER_HPP

#define DEFAULT_DIR_NAME "files"

#include "MessageHandler.hpp"

class RequestParser {

	private:
		const WebservConfig&	_config;
		HttpRequest*			_request;
		std::string				_raw_request;
		size_t					_current_pos;
		Status					_last_status;

		int						_port;
		const ServerConfig*		_server_config;
		
		bool			parseRequestLine();
		bool			parseMethod(std::string request_line);
		bool			parseUri(std::string request_line);
		bool			parseVersion(std::string request_line);
		bool			parseHeaders();
		std::string		parseHeaderName(std::string line);
		bool			parseBody();

		std::string		findLocationName(const std::string& path);
		
	public:
		RequestParser(const WebservConfig& config, HttpRequest* request, const std::string& raw_request, const int& port);
		RequestParser(const RequestParser& rhs);
		RequestParser& operator=(const RequestParser& rhs);
		~RequestParser();

		bool	parseRequest();
		void	setRequestContext();
		Status	getLastStatus() const;

};

bool parseHeaderLine(const std::string& header_line, std::string& name, std::vector<std::string>& values);

#endif //REQUESTPARSER_HPP