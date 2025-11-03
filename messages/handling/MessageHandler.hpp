#ifndef MESSAGEHANDLER_HPP
#define MESSAGEHANDLER_HPP

#include "../../config/WebservConfig.hpp"
#include "../../parsing/Parsing.hpp"
#include "../../status/status.hpp"
#include "../data/HttpMessage.hpp"
#include "../data/HttpRequest.hpp"
#include "../data/HttpResponse.hpp"
#include "RequestParser.hpp"
#include "RequestValidator.hpp"
#include "RequestProcessor.hpp"
#include "ResponseGenerator.hpp"

class eventProcessor;

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

		bool			parseRequest(const std::string& raw_request, const int& port);
		bool			validateRequest();
		void			processRequest();
		void			generateResponse();
		std::string		serializeResponse();

		Status			getLastStatus() const;
		void			setLastStatus(Status status);
};

std::string handle_messages(const WebservConfig& config, const std::string& raw_request, int port, eventProcessor *evProc, int clientFd);

#endif //MESSAGEHANDLER_HPP
