#ifndef MESSAGEVALIDATOR_HPP
#define MESSAGEVALIDATOR_HPP

#include "../data/HttpMessage.hpp"
#include "../data/HttpRequest.hpp"
#include "../data/HttpResponse.hpp"
#include "../../config/WebservConfig.hpp"
#include "../../parsing/Parsing.hpp"
#include "../../status/status.hpp"

#include <sys/stat.h>

#define MAX_URI_LENGTH 8000
#define MAX_HEADERS_SIZE 8000

class MessageValidator {

	private:
		const WebservConfig&	_config;
		HttpRequest*			_request;
		Status					_last_status;
		std::vector<std::string>	_host_header;

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

	public:
		MessageValidator(const WebservConfig& config, HttpRequest* request);
		MessageValidator(const MessageValidator& rhs);
		MessageValidator& operator=(const MessageValidator& rhs);
		~MessageValidator();

		bool	validateRequest();
		Status	getLastStatus() const;
};

std::string	canonicalize_path(const std::string& path);
bool		is_within_root(const std::string& resolved_path, const std::string& document_root);
bool		contains_traversal(const std::string& path);
std::string	extract_relative_path(const std::string& full_path, const std::string& location_prefix);
std::string	build_full_path(const std::string& root, const std::string& relative_path);
std::string	resolve_index_file(const std::string& directory_path, const std::string& index_file);

#endif // MESSAGEVALIDATOR_HPP