#ifndef MESSAGEHANDLER_HPP
#define MESSAGEHANDLER_HPP

#include "../../config/WebservConfig.hpp"
#include "../../status/status.hpp"
#include "../data/HttpMessage.hpp"
#include "../data/HttpRequest.hpp"
#include "../data/HttpResponse.hpp"
#include "../parsing/RequestParser.hpp"
#include "RequestProcessor.hpp"
#include "MessageValidator.hpp"
#include "ResponseGenerator.hpp"

class MessageHandler {

	private:
		MessageValidator*		_validator;

	protected:
		const WebservConfig&	_config;
		HttpRequest				_request;
		HttpResponse			_response;
		Status					_last_status;

	public:
		MessageHandler(const WebservConfig& config);
		MessageHandler(const MessageHandler& rhs);
		MessageHandler& operator=(const MessageHandler& rhs);
		virtual ~MessageHandler();

		bool	parseRequest(const std::string& raw_request);
		bool	validateRequest();
		bool	processRequest();
		bool	generateResponse();

		Status	getLastStatus() const;
		
		const HttpRequest	getRequest() const;
		const HttpResponse	getResponse() const;
};

void	handle_messages(const WebservConfig& config, const std::string &raw_request);

#endif //MESSAGEHANDLER_HPP