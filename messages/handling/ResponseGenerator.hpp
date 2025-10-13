#ifndef RESPONSEGENERATOR_HPP
#define RESPONSEGENERATOR_HPP

#include <dirent.h>
#include "MessageHandler.hpp"
#include "../../config/WebservConfig.hpp"

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
		
	public:
		ResponseGenerator(const WebservConfig& config, HttpRequest* request, HttpResponse* response, Status status);
		ResponseGenerator(const ResponseGenerator& rhs);
		ResponseGenerator& operator=(const ResponseGenerator& rhs);
		~ResponseGenerator();

		Status			getLastStatus() const;
		void			generateResponse();
		void			setDefaultHeaders();
		void			setContentHeaders();
 		void			setDateHeader();
 		std::string		getCurrentHTTPDate() const;
		std::string		readFileContent(std::ifstream& file) const;
		std::string		generateDirectoryHTML();
		std::string		generateDefaultErrorHTML();
		std::string		generateRedirHTML();
		bool			isValidCGI() const;
		void			addValidIndex();
};

std::string		getMimeType(const std::string& extension);
Status			findErrorStatus(const std::string& path);

#endif // RESPONSEGENERATOR_HPP