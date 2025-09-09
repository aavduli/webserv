#ifndef HTTPHEADERS_HPP
#define HTTPHEADERS_HPP

#include <iostream>
#include <string>
#include <algorithm>
#include <map>
#include <vector>

#include "../parsing/Parsing.hpp"

/* 
   HTTP 1.1
   message-header = field-name ":" [ field-value ]
   field-name     = token
   field-value    = *( field-content | LWS )
   field-content  = <the OCTETs making up the field-value
                    and consisting of either *TEXT or combinations
                    of token, separators, and quoted-string>
 */

class HttpHeaders {

	private:
		// key-value pairs of all header fields, a key can have multiple values
		std::map<std::string, std::vector<std::string> >	_fields;
		
	public:
		HttpHeaders();		// should be private?
		HttpHeaders(const std::map<std::string, std::vector<std::string> >& fields);
		HttpHeaders(const HttpHeaders& rhs);
		HttpHeaders& operator=(const HttpHeaders& rhs);
		~HttpHeaders();

		bool		hasHeader(const std::string& key) const;
		void		addHeader(const std::string& key, const std::string& value);
		std::vector<std::string>	getHeaderValues(const std::string& key) const;
		void		setHeaderValues(const std::string& key, const std::string& value);
};


#endif // HTTPHEADERS_HPP
