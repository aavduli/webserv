#ifndef REQUESTVALIDATOR_HPP
#define REQUESTVALIDATOR_HPP

#include "../data/HttpMessage.hpp"
#include "../data/HttpRequest.hpp"
#include "../data/HttpResponse.hpp"
#include "../../config/WebservConfig.hpp"
#include "../../parsing/Parsing.hpp"
#include "../../status/status.hpp"

#include <sys/stat.h>

#define MAX_URI_LENGTH 8000
#define MAX_HEADERS_SIZE 8000

class RequestValidator {

	private:
		const WebservConfig&		_config;
		HttpRequest*				_request;
		Status						_last_status;
		std::vector<std::string>	_host_header;

		bool	validateVersion();
		bool	validateHost();
		bool	validatePort();
		bool	validateMethod();
		bool	validatePath();
		bool	validateContentLength();
		bool	validateHeaderLimits();
		bool	validateRedirection();

	public:
		RequestValidator(const WebservConfig& config, HttpRequest* request);
		RequestValidator(const RequestValidator& rhs);
		RequestValidator& operator=(const RequestValidator& rhs);
		~RequestValidator();

		bool	validateRequest();
		Status	getLastStatus() const;
};

#endif // REQUESTVALIDATOR_HPP