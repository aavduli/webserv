#ifndef REQUESTURI_HPP
#define REQUESTURI_HPP

#include <string>
#include <iostream>

#include "../../console/console.hpp"
#include "../../parsing/Parsing.hpp"
#include "../../config/WebservConfig.hpp"

class RequestUri {

	private:
		std::string		_raw_uri;
		std::string		_scheme;		// (protocol) https + ://
		std::string		_userinfo;		// (optional) john.doe@
		std::string		_host;			// (optional) www.website.com + :
		std::string		_port;			// (optional) 8080 + /
		std::string		_path;			// blog/posts + ?
		std::string		_full_path;		// root + path + file
		std::string		_query;			// (optional) order=newest + #
		std::string		_fragment;		// (optional) top
		bool			_is_absolute_uri;
		bool			_is_abs_path;
		
		std::string		extract_uri_component(size_t* pos, const std::string& str, const std::string& del);
		bool			is_absolute_uri(const std::string& uri);
		void			clear_uri();
		bool			parse_uri_path_query(const std::string& raw);
		bool			parse_uri_authority(const std::string& raw);
		RequestUri		parse_absolute_uri(const std::string& raw);
		RequestUri		parse_abs_path(const std::string& raw);
		
	public:
		RequestUri();
		RequestUri(const std::string& raw_uri);
		RequestUri(const RequestUri& rhs);
		RequestUri& operator=(const RequestUri& rhs);
		~RequestUri();
		
		bool			parse();
		void			print() const;

		// Getters
		std::string		getRawUri() const;
		std::string		getScheme() const;
		std::string		getUserinfo() const;
		std::string		getHost() const;
		std::string		getPort() const;
		std::string		getPath() const;
		std::string		getFullPath() const;
		std::string		getQuery() const;
		std::string		getFragment() const;
		bool			isAbsoluteUri() const;
		bool			isAbsPath() const;

		// Setters
		void			setRawUri(const std::string& raw_uri);
		void			setScheme(const std::string& scheme);
		void			setUserinfo(const std::string& userinfo);
		void			setHost(const std::string& host);
		void			setPort(const std::string& port);
		void			setPath(const std::string& path);
		void			setFullPath(const std::string& path);
		void			setQuery(const std::string& query);
		void			setFragment(const std::string& fragment);
		void			setAbsoluteUri(bool is_absolute_uri);
		void			setAbsPath(bool is_abs_path);
};

#endif //REQUESTURI_HPP