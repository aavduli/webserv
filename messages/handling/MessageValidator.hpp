#ifndef MESSAGEVALIDATOR_HPP
#define MESSAGEVALIDATOR_HPP

#include "../data/HttpMessage.hpp"
#include "../data/HttpRequest.hpp"
#include "../data/HttpResponse.hpp"
#include "../../config/WebservConfig.hpp"
#include "../../parsing/Parsing.hpp"
#include "../../errors/errors.hpp"

#include <sys/stat.h>

#define MAX_URI_LENGTH 8000
#define MAX_HEADERS_SIZE 8000

class MessageValidator {
	private:
		const WebservConfig&				_config;
		HttpRequest&						_request;
		Error								_last_error;
		std::map<std::string, std::string>	_location_config;
		std::vector<std::string>			_host_header;

	public:
		MessageValidator(const WebservConfig& config, HttpRequest& request);
		
		bool	isValidRequest();
		Error	getLastError() const;

	private:
		bool	validateHost();
		bool	validatePort();
		bool	validateMethod();
		bool	validateVersion();
		bool	validateBodySize();
		bool	validatePath();
		bool	validateTransferEncoding();
		bool	validateContentType();
		bool	validateHeaderLimits();
		bool	validateExpectHeader();
		bool	validateConnectionHeader();
		bool	validateRedirection();
};

bool	is_valid_body_size(const size_t& size, const std::string& max_config);

std::string	canonicalize_path(const std::string& path);
bool		is_within_root(const std::string& resolved_path, const std::string& document_root);
bool		contains_traversal(const std::string& path);

#endif // MESSAGEVALIDATOR_HPP