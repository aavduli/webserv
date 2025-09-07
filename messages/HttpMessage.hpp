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
	s_msg_dead = 1,
	s_msg_empty,
	s_msg_error,
	s_msg_init,
	s_msg_version,

	/* REQUEST */
	s_req_start, 
	s_req_method, 
	s_req_uri, 
	s_req_version, 
	s_req_done, 

	/* RESPONSE */
	s_res_start,
	s_res_protocol,
	s_res_version,
	s_res_status,
	s_res_reason,
	s_res_done,

	/* HEADERS */
	s_head_start,
	s_head_fields,
	s_head_done,

	/* BODY */
	s_body_start,
	s_body_content,
	s_body_done,	// msg_done directly?

	s_msg_done
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
		void		setHttpVersion(double version);

		HttpHeaders	getHeaders() const;
		void		setHeaders(const std::map<std::string, std::vector<std::string> >& fields);

		std::string	getBody() const;
		void		setBody(const std::string& body);
};

#endif // HTTPMESSAGE_HPP
