#ifndef HTTPMESSAGE_HPP
#define HTTPMESSAGE_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <map>
#include <vector>

#include "../../status/status.hpp"
#include "../../console/console.hpp"
#include "../parsing/MessageParser.hpp"

class HttpRequest;
class HttpResponse;

class HttpMessage {

	protected:
		std::string		_version_major;
		std::string		_version_minor;
		std::map<std::string, std::vector<std::string> >	_headers;
		size_t			_headers_size;
		std::string		_body;
		size_t			_body_size;
		
	public:
		HttpMessage();
		HttpMessage(const HttpMessage& rhs);
		HttpMessage& operator=(const HttpMessage& rhs);
		virtual ~HttpMessage();

		std::string	getHttpVersion() const;
		std::string	getHttpVersionMajor() const;
		std::string	getHttpVersionMinor() const;
		void		setHttpVersion(std::string major, std::string minor);

		bool		hasHeader(const std::string& key) const;
		void		addHeader(const std::string& key, const std::vector<std::string>& values);
		std::map<std::string, std::vector<std::string> >	getHeaders() const;
		const std::vector<std::string>&	getHeaderValues(const std::string& key) const;
		void 		printHeaders() const;
		size_t		getHeadersSize() const;
		void		setHeadersSize(size_t headers_size);

		std::string	getBody() const;
		void		setBody(const std::string& body);
		size_t		getBodySize() const;
		void		setBodySize(size_t body_size);
};

#endif // HTTPMESSAGE_HPP
