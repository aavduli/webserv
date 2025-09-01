#ifndef PARSER_RESPONSE_HPP
#define PARSER_RESPONSE_HPP

#include "parser_message.hpp"

enum ResponseStatus {
	OK = 200
};

class HttpResponse : public HttpMessage {

	private:
		std::map<int, std::string>	status;		// status code (key) = reason phrase (value)
												// OR
		int			status_code;				// code stored alone as int
		std::string	reason_phrase;				// short textual description associated to the status code
												// OR
		ResponseStatus	status;					// enum STR = int

	public:
		HttpResponse();
		HttpResponse(const HttpResponse& rhs);
		HttpResponse& operator=(const HttpResponse& rhs);
		~HttpResponse();

};

#endif // PARSER_RESPONSE_HPP