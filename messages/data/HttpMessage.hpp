#ifndef HTTPMESSAGE_HPP
#define HTTPMESSAGE_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <map>
#include <vector>

#include "../../errors/errors.hpp"
#include "../../console/console.hpp"
#include "../parsing/MessageParser.hpp"

class HttpRequest;
class HttpResponse;

class HttpMessage {

	protected:
		State			_state;
		std::string		_version_major;
		std::string		_version_minor;
		std::map<std::string, std::vector<std::string> >	_headers;
		std::string		_body;
		size_t			_content_length;
		
	public:
		HttpMessage();
		HttpMessage(const HttpMessage& rhs);
		HttpMessage& operator=(const HttpMessage& rhs);
		virtual ~HttpMessage();

		State		getState() const;
		void		setState(State state);
		std::string	getHttpVersion() const;
		void		setHttpVersion(std::string major, std::string minor);

		bool		hasHeader(const std::string& key) const;
		void		addHeader(const std::string& key, const std::vector<std::string>& values);
		std::vector<std::string>	getHeaderValues(const std::string& key) const;
		void 		printHeaders() const;

		std::string	getBody() const;
		void		setBody(const std::string& body);

		size_t		getContentLength() const;
		void		setContentLength(size_t content_length);
};

#endif // HTTPMESSAGE_HPP
