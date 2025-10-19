#ifndef RESPONSEGENERATOR_HPP
#define RESPONSEGENERATOR_HPP

#include <dirent.h>
#include "MessageHandler.hpp"
#include "../../config/WebservConfig.hpp"
#include "../../console/console.hpp"
#include <ctime>
#include <sstream>

class ResponseGenerator {

	private:
		const WebservConfig&	_config;
		HttpRequest*			_request;
		HttpResponse*			_response;
		Status					_last_status;
		bool					_done;
		
		void	generateStaticFileResponse();
		void	generateRedirResponse();
		void	generateCGIResponse();
		void	generateDirectoryResponse();
		void	generateErrorResponse();

		void	generatePostResponse();

	public:
		ResponseGenerator(const WebservConfig& config, HttpRequest* request, HttpResponse* response, Status status);
		ResponseGenerator(const ResponseGenerator& rhs);
		ResponseGenerator& operator=(const ResponseGenerator& rhs);
		~ResponseGenerator();

		Status			getLastStatus() const;
		bool			isValidCGI() const;
		void			addValidIndex();
		
		void			generateResponse();
		std::string		generateDirectoryHTML();
		std::string		generateDefaultErrorHTML();
		std::string		generateRedirHTML();
		void			setHeaders();
};

std::string		getMimeType(const std::string& extension);
Status			findErrorStatus(const std::string& path);
std::string 	getCurrentGMTDate();

#endif // RESPONSEGENERATOR_HPP