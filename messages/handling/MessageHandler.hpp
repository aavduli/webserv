#ifndef MESSAGEHANDLER_HPP
#define MESSAGEHANDLER_HPP

#include "../../config/WebservConfig.hpp"
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

		std::string							findConfigLocationName();
		std::map<std::string, std::string>	findLocationMatch();

	public:
		MessageHandler(const WebservConfig& config, HttpRequest* request);
		MessageHandler(const MessageHandler& rhs);
		MessageHandler& operator=(const MessageHandler& rhs);
		virtual ~MessageHandler();
		
		bool			parseRequest(const std::string& raw_request);
		void			setRequestContext();
		bool			validateRequest();
		bool			generateResponse();
		std::string		serializeResponse();

		Status	getLastStatus() const;
		void	setLastStatus(Status status);
};

const std::string&	handle_messages(const WebservConfig& config, const std::string &raw_request);

#endif //MESSAGEHANDLER_HPP