#ifndef MESSAGEHANDLER_HPP
#define MESSAGEHANDLER_HPP

#include "../../config/WebservConfig.hpp"
#include "../../parsing/Parsing.hpp"
#include "../../status/status.hpp"
#include "../data/HttpMessage.hpp"
#include "../data/HttpRequest.hpp"
#include "../data/HttpResponse.hpp"
#include "../parsing/RequestParser.hpp"
#include "RequestValidator.hpp"
#include "ResponseGenerator.hpp"

class MessageHandler {

	private:
		const WebservConfig&	_config;
		HttpRequest*			_request;
		HttpResponse			_response;
		Status					_last_status;

	public:
		MessageHandler(const WebservConfig& config, HttpRequest* request);
		MessageHandler(const MessageHandler& rhs);
		MessageHandler& operator=(const MessageHandler& rhs);
		virtual ~MessageHandler();
		
		bool			parseRequest(const std::string& raw_request);
		void			validateRequest();
		void			generateResponse();
		std::string		serializeResponse();

		Status			getLastStatus() const;
		void			setLastStatus(Status status);
};

std::string	handle_messages(const WebservConfig& config, const std::string& raw_request);

#endif //MESSAGEHANDLER_HPP