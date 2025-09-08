#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include "HttpMessage.hpp"

enum HttpMethod {
	GET,		// 0
	POST,		// 1
	DELETE,		// 2
	HEAD,		// 3
	PUT,		// 4
	CONNECT,	// 5
	OPTIONS,	// 6
	TRACE,		// 7
};

/* 
REQUEST FORMAT
	GET /path/to/resource HTTP/1.1
	Host: localhost:8080
	User-Agent: Mozilla/5.0
	Accept: text/html
	Content-Length: 123

	[Optional body]

TODO: Parse incoming HTTP request from buffer
	HttpRequest request(buffer);

TODO: Create HTTP response with status code, headers and output body if needed
	HttpResponse response(request);
 */

class HttpRequest : public HttpMessage {

	private:
		std::string	_method;
		std::string	_uri;		// needs to be a class?

	public:
		HttpRequest(std::string raw_request);
		HttpRequest(const HttpRequest& rhs);
		HttpRequest& operator=(const HttpRequest& rhs);
		~HttpRequest();

		std::string	getMethod() const;
		std::string	getUri() const;

		bool	parse_request(std::string raw_request);
		bool	parse_req_method(std::string::const_iterator& it, std::string::const_iterator end);
		bool	parse_req_uri(std::string::const_iterator& it, std::string::const_iterator end);
		bool	parse_req_version(std::string::const_iterator& it, std::string::const_iterator end);
		bool	parse_req_headers(std::string::const_iterator& it, std::string::const_iterator end);
		bool	parse_req_body(std::string::const_iterator& it, std::string::const_iterator end);
		bool	is_valid_request();
};
// 
// void		get_simple_request(s_msg_streams streams);
// void		get_chunked_request(s_msg_streams streams);
// 
// std::string	get_stream_line(std::ostream& stream);
// 
// void		print_request(HttpRequest request);

#endif // HTTPREQUEST_HPP