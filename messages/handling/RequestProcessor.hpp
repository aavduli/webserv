#ifndef REQUESTPROCESSOR_HPP
#define REQUESTPROCESSOR_HPP

#include "../data/HttpMessage.hpp"
#include "../data/HttpRequest.hpp"
#include "../data/HttpResponse.hpp"
#include "../parsing/RequestParser.hpp"
#include "../../config/WebservConfig.hpp"
#include "../../console/console.hpp"
#include "../../parsing/Parsing.hpp"
#include "../../status/status.hpp"
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#define DEFAULT_BUFFER_SIZE 8192

class RequestProcessor {

	private:
		const WebservConfig&		_config;
		HttpRequest*				_request;
		Status						_last_status;
		
		void			processURLEncodedBody();

		void						processMultipartBody();
		bool						processMultipartPart(std::string& part, std::map<std::string, PostData>& data, const std::string& boundary);
		std::string					extractBoundary(const std::vector<std::string>& ct_values);
		std::vector<std::string>	splitByBoundary(const std::string& body, const std::string& boundary);
		std::map<std::string, std::vector<std::string> >	parseMultipartHeaders(std::string& header_str, const std::string& boundary);
		std::string					extractDispositionData(const std::map<std::string, std::vector<std::string> >& headers, const std::string& key);

		bool			processFileUpload(PostData& value);
		bool			configUploadDir();
		bool			writeFileUploads(const std::string& filename, PostData& file_data);

	public:
		RequestProcessor(const WebservConfig& config, HttpRequest* request);
		RequestProcessor(const RequestProcessor& rhs);
		RequestProcessor& operator=(const RequestProcessor& rhs);
		~RequestProcessor();

		bool		processPostRequest();
		bool		processDeleteRequest();
		Status		getLastStatus() const;
};

ssize_t			write_on_fd(const int fd, const std::string& content, size_t &pos, size_t buf_size);
std::string		generateFilename(const std::string& og_name);

#endif // REQUESTPROCESSOR_HPP