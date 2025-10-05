#ifndef RESPONSEGENERATOR_HPP
#define RESPONSEGENERATOR_HPP

#include "MessageHandler.hpp"
#include "../../config/WebservConfig.hpp"

class ResponseGenerator {

	private:
		const WebservConfig&	_config;
		HttpRequest*			_request;
		HttpResponse*			_response;
		Status					_last_status;

//		std::string		serializeResponse();
		
// 		void			generateStaticFileResponse();
// 		void			generateDirectoryListingResponse();
// 		void			generateRedirectResponse();
// 		void			generateErrorResponse();
// 		void			generateCGIResponse();

	public:
		ResponseGenerator(const WebservConfig& config, HttpRequest* request, HttpResponse* response);
		ResponseGenerator(const ResponseGenerator& rhs);
		ResponseGenerator& operator=(const ResponseGenerator& rhs);
		~ResponseGenerator();

		Status	getLastStatus() const;
		void	generateResponse();

// 		void			setDefaultHeaders();
// 		void			setConnectionHeader();
// 		void			setContentHeaders(size_t content_length);
// 		void			setDateHeader();
// 		
// 		bool			shouldCloseConnection() const;
// 		std::string		getCurrentHTTPDate() const;
// 		std::string		getMimeType(const std::string& file_extension) const;
// 		
// 		std::string		generateDirectoryHTML(const std::string& directory_path) const;
// 		std::string		readFileContent(const std::string& file_path) const;

};

#endif // RESPONSEGENERATOR_HPP