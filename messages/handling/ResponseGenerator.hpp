#ifndef RESPONSEGENERATOR_HPP
#define RESPONSEGENERATOR_HPP

#include <dirent.h>
#include "MessageHandler.hpp"
#include "../../config/WebservConfig.hpp"
#include "../../console/console.hpp"
#include "../../cgi/CgiExec.hpp"
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
		void	generateDeleteResponse();
		//void	parseCGIOutput(const std::string& cgi_output);

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
		std::string		generatePostSuccessHTML();
		void			setHeaders();
		void			parseCGIOutput(const std::string& cgi_output);
		std::string		readFileContent(std::ifstream& file) const;
};

std::string		getMimeType(const std::string& extension);
Status			findErrorStatus(const std::string& path);
std::string 	getCurrentGMTDate();

#endif // RESPONSEGENERATOR_HPP
