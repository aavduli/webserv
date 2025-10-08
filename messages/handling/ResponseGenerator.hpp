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
		
		void			generateStaticFileResponse(const std::string& path);
		void			generateRedirResponse();
		void			generateCGIResponse();
		void			generateDirectoryResponse(const std::string& path);
		void			generateErrorResponse();

	public:
		ResponseGenerator(const WebservConfig& config, HttpRequest* request, HttpResponse* response);
		ResponseGenerator(const ResponseGenerator& rhs);
		ResponseGenerator& operator=(const ResponseGenerator& rhs);
		~ResponseGenerator();

		Status	getLastStatus() const;
		void	setLastStatus(Status last_status);

		void	generateResponse();

		void			setDefaultHeaders();
// 		void			setConnectionHeader();
// 		void			setDateHeader();
// 		
// 		bool			shouldCloseConnection() const;
// 		std::string		getCurrentHTTPDate() const;
		std::string		getMimeType(const std::string& extension) const;
// 		
		std::string		generateDirectoryListing(const std::string& path) const;
		// std::string		readFileContent(const std::string& file_path) const;
};

template<typename T>
std::string nb_to_string(T value);

#endif // RESPONSEGENERATOR_HPP