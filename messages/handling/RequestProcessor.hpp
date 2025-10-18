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

class RequestProcessor {

	private:
		const WebservConfig&		_config;
		HttpRequest*				_request;
		Status						_last_status;
		
		void			processURLEncodedBody();
		std::string		urlDecode(const std::string& encoded);

		void						processMultipartBody();
		bool						processMultipartPart(const std::string& part, std::map<std::string, PostData>& data);
		std::string					extractBoundary(const std::vector<std::string>& ct_values);
		std::vector<std::string>	splitByBoundary(const std::string& body, const std::string& boundary);
		std::map<std::string, std::vector<std::string> >	parseMultipartHeaders(const std::string& header_str);
		std::string					extractDispositionData(const std::map<std::string, std::vector<std::string> >& headers, const std::string& key);

		void			processFileUpload();
		bool			configUploadDir();
		std::string		generateFilename(const std::string& og_name);
		bool			writeFileToDisk(const std::string& filename);
		bool			writeFileToDisk(const std::string& filename, const PostData& file_data);

	public:
		RequestProcessor(const WebservConfig& config, HttpRequest* request);
		RequestProcessor(const RequestProcessor& rhs);
		RequestProcessor& operator=(const RequestProcessor& rhs);
		~RequestProcessor();

		bool		processPostRequest();
		bool		processDeleteRequest();
		Status		getLastStatus() const;
};

#endif // REQUESTPROCESSOR_HPP