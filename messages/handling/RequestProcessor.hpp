#ifndef REQUESTPROCESSOR_HPP
#define REQUESTPROCESSOR_HPP

#include "MessageHandler.hpp"

class MessageValidator;

class RequestProcessor {

	private:
		const WebservConfig&	_config;
		const HttpRequest&		_request;
		Status					_last_status;

		bool	validateGetRequest();
		bool	validatePostRequest();
		bool	validateDeleteRequest();
		void	processGetRequest();
		void	processPostRequest();
		void	processDeleteRequest();

	public:
		RequestProcessor(const WebservConfig& config, const HttpRequest& request);
		RequestProcessor(const RequestProcessor& rhs);
		RequestProcessor& operator=(const RequestProcessor& rhs);
		~RequestProcessor();

		void	processRequest();
		Status	getLastStatus() const;
};

#endif // REQUESTPROCESSOR_HPP