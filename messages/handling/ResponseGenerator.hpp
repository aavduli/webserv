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

		std::string		serializeResponse();
		
		void			generateStaticFileResponse();
		void			generateRedirResponse();
		void			generateCGIResponse();
		void			generateDirectoryResponse();
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

std::string get_mime_type(const std::string& extension);
template<typename T>
std::string nb_to_string(T value);

#endif // RESPONSEGENERATOR_HPP