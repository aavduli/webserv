#ifndef RESPONSEHANDLER_HPP
#define RESPONSEHANDLER_HPP

#include "MessageHandler.hpp"
#include "../../config/WebservConfig.hpp"

class ResponseHandler : public MessageHandler {

	private:
		const WebservConfig&	_config;

	public:
		ResponseHandler(HttpRequest* request, const WebservConfig& config);
		ResponseHandler(const ResponseHandler& rhs);
		ResponseHandler& operator=(const ResponseHandler& rhs);
		~ResponseHandler();

		void			generateResponse();
		//std::string		serializeResponse();
		
// 		void			generateStaticFileResponse();
// 		void			generateDirectoryListingResponse();
// 		void			generateRedirectResponse();
// 		void			generateErrorResponse();
// 		void			generateCGIResponse();
// 
// 	private:
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

#endif //RESPONSEHANDLER_HPP