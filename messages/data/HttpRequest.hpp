#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include "HttpMessage.hpp"
#include "RequestUri.hpp"

class WebservConfig;

struct PostData {
	std::string		filename;
	std::string		new_filename;
	std::string		content;
	size_t			bytes_written;	// to remove
	std::string		content_type;
	bool			is_file;

	PostData() : bytes_written(0), is_file(false) {}
	PostData(const std::string& simple_content) : content(simple_content), bytes_written(0), is_file(false) {}	// URL-encoded data
};

struct RequestContext {
	
	public:
		std::string							_location_name;
		std::map<std::string, std::string>	_location_config;
		std::string							_document_root;
		std::vector<std::string>			_index_list;
		bool								_autoindex_enabled;
		bool								_has_redirect;
		bool								_upload_enabled;
		std::string							_upload_dir;
};

class HttpRequest : public HttpMessage {

	private:
		std::string							_method;
		RequestUri							_uri;
		std::map<std::string, PostData>		_post_data;

	public:
		RequestContext	ctx;
		
		HttpRequest();
		HttpRequest(const HttpRequest& rhs);
		HttpRequest& operator=(const HttpRequest& rhs);
		~HttpRequest();

		const std::string&						getMethod() const;
		void									setMethod(const std::string& method);
		RequestUri								getUri() const;
		void									setUri(const RequestUri& uri);
		std::map<std::string, PostData>			getPostData() const;
		void									setPostData(const std::map<std::string, PostData>& post_data);
};

void		print_request(HttpRequest* request);

#endif // HTTPREQUEST_HPP
