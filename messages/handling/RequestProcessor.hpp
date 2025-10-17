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

class RequestProcessor {

	private:
		const WebservConfig&		_config;
		HttpRequest*				_request;
		Status						_last_status;
		
		bool			decodePostBody();
		void			processURLEncodedBody();
		std::string		urlDecode(const std::string& encoded);

		void										processMultipartBody();
		std::string									extractBoundary(const std::vector<std::string>& ct_values);
		std::vector<std::string>					splitByBoundary(const std::string& body, const std::string& boundary);
		std::map<std::string, std::vector<std::string> >	parseMultipartHeaders(const std::string& header_str);
		std::string									extractDispositionData(const std::map<std::string, std::vector<std::string> >& headers, const std::string& key);

		bool	isFileUpload();
		bool	processFileUpload();
		bool	checkUploadPermissions();

		bool	isCGI();
		bool	processCGI();

	public:
		RequestProcessor(const WebservConfig& config, HttpRequest* request);
		RequestProcessor(const RequestProcessor& rhs);
		RequestProcessor& operator=(const RequestProcessor& rhs);
		~RequestProcessor();

		bool	processPostRequest();
		bool	processDeleteRequest();
		Status	getLastStatus() const;
};

#endif // REQUESTPROCESSOR_HPP