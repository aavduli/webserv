#ifndef MESSAGEPARSER_HPP
#define MESSAGEPARSER_HPP

#include <iostream>
#include <string>
#include <algorithm>
#include <map>
#include <vector>

#define CR	'\r'
#define LF	'\n'
#define MAX_body_size 1000000
#define MAX_URI_LENGTH 8000

#include "../../console/console.hpp"
#include "../../config/WebservConfig.hpp"
#include "../../status/status.hpp"
#include "../../parsing/Parsing.hpp"

class HttpMessage;
class HttpRequest;
class HttpResponse;

class MessageParser {

	protected:
		Status			_status;
		std::string		_raw_data;		// raw_request
		size_t			_current_pos;	// pos in raw_data string

	public:
		MessageParser();
		MessageParser(const MessageParser& rhs);
		MessageParser& operator=(const MessageParser& rhs);
		virtual ~MessageParser();

		Status		getStatus() const;
};

// RESPONSE PARSER (inherits from MessageParser)
class ResponseParser : public MessageParser {

	private:
		HttpResponse*	_response;		// for response parsing

	public:
		ResponseParser();
		ResponseParser(const ResponseParser& rhs);
		ResponseParser& operator=(const ResponseParser& rhs);
		~ResponseParser();
};

#endif //MESSAGEPARSER_HPP
