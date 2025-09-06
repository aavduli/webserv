#ifndef HTTPMESSAGE_HPP
#define HTTPMESSAGE_HPP

#include <iostream>
#include <string>
#include <algorithm>
#include <map>
#include <vector>

#define CR	'\r'
#define LF	'\n'

#include "../parsing/Parsing.hpp"
#include "HttpHeaders.hpp"

enum State {
	S_INIT = -1,
	S_SUCCESS = 0,
	S_ERROR = 1
};

class HttpMessage {

	protected:
		State			_state;			// monitor
		double			_http_version;	// <major>.<minor> format
		HttpHeaders		_headers;		// general, request, response, entity header fields
		std::string		_body;			// optional

	public:
		HttpMessage();					// not instanciable
		HttpMessage(const HttpMessage& rhs);
		HttpMessage& operator=(const HttpMessage& rhs);
		virtual ~HttpMessage();			// base class

		void		setState(State state);
		State		getState() const;

		double		getHttpVersion() const;
		void		setHttpVersion(double& version);

		HttpHeaders	getHeaders() const;
		void		setHeaders(const std::map<std::string, std::vector<std::string> >& fields);

		std::string	getBody() const;
		void		setBody(const std::string& body);
};

#endif // HTTPMESSAGE_HPP
