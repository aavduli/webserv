#ifndef PARSER_MESSAGE_HPP
#define PARSER_MESSAGE_HPP

#include <iostream>
#include <string>
#include <map>

#define CR	'\r'
#define LF	'\n'

#include "parser_request.hpp"
#include "parser_response.hpp"

// General config classes, structs and enums

/* Header names are case-insensitive -> need normalization
   Some headers can have multiple values. */

class HttpHeaders {

	private:
		std::map<std::string, std::string>	fields;		// key-value pairs of all header fields

	public:
		HttpHeaders();
		HttpHeaders(const HttpHeaders& rhs);
		HttpHeaders& operator=(const HttpHeaders& rhs);
		~HttpHeaders();

		std::string	getValue(const std::string& key) const;
		void		setValue(const std::string& key, const std::string& value);

};

class HttpMessage {

	private:
		double			http_version;	// <major>.<minor> format
		HttpHeaders		headers;		// general, request, response, entity header fields
		std::string		body;			// optional

	public:
		HttpMessage();
		HttpMessage(const HttpMessage& rhs);
		HttpMessage& operator=(const HttpMessage& rhs);
		virtual ~HttpMessage();			// base class

		double		getHttpVersion() const;
		void		setHttpVersion(double& version);

		HttpHeaders	getHeaders() const;
		void		setHeaders(const std::map<std::string, std::string>& fields);

		std::string	getBody() const;
		void		setBody(const std::string& body) const;

};

#endif // PARSER_MESSAGE_HPP
