#ifndef RESPONSEGENERATOR_HPP
#define RESPONSEGENERATOR_HPP

#include <dirent.h>
#include "MessageHandler.hpp"
#include "../../config/WebservConfig.hpp"
#include "../../console/console.hpp"
#include "../../cgi/CgiExec.hpp"
#include "../../server/EventProcessor.hpp"
#include <ctime>
#include <sstream>

struct HTMLTemplate {

	std::string	title;
	std::string	subtitle;
	std::string	content;
	std::string	card_type;			// "error", "form", ""
	std::string	page_type;			// "error-page", ""
	bool		has_main_title;		// true for forms/file manager, false for errors

	HTMLTemplate(const std::string& t = "", const std::string& st = "",
				const std::string& c = "", const std::string& ct = "",
				const std::string& pt = "", bool hmt = false)
		: title(t), subtitle(st), content(c), card_type(ct), page_type(pt), has_main_title(hmt) {}

	std::string render() const;
};

class ResponseGenerator {

	private:
		const WebservConfig&	_config;
		HttpRequest*			_request;
		HttpResponse*			_response;
		Status					_last_status;

		void	generateRedirResponse();
		void	generatePostResponse();
		void	generateDeleteResponse();
		void	generateCGIResponse();
		void	generateDirectoryResponse();
		void	generateStaticFileResponse();
		void	generateErrorResponse();

	public:
		ResponseGenerator(const WebservConfig& config, HttpRequest* request, HttpResponse* response, Status status);
		ResponseGenerator(const ResponseGenerator& rhs);
		ResponseGenerator& operator=(const ResponseGenerator& rhs);
		~ResponseGenerator();

		Status			getLastStatus() const;
		void			generateResponse();
		bool			isValidCGI() const;
		void			addValidIndex();
		void			setHeaders();
		void			parseCGIOutput(const std::string& cgi_output);
};

std::string		getMimeType(const std::string& extension);
Status			findErrorStatus(const std::string& path);
std::string 	getCurrentGMTDate();

#endif // RESPONSEGENERATOR_HPP
