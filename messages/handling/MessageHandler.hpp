#ifndef MESSAGEHANDLER_HPP
#define MESSAGEHANDLER_HPP

#include "../../status/status.hpp"
#include "../parsing/MessageParser.hpp"
#include "../parsing/RequestParser.hpp"
#include "../data/HttpMessage.hpp"
#include "../data/HttpRequest.hpp"
#include "../data/HttpResponse.hpp"
#include "../../config/WebservConfig.hpp"

class MessageValidator;

class MessageHandler {

	private:
		Status					_last_status;
		HttpRequest*			_request;
		HttpResponse*			_response;

	public:
		MessageHandler(HttpRequest* request);
		MessageHandler(const MessageHandler& rhs);
		MessageHandler& operator=(const MessageHandler& rhs);
		virtual ~MessageHandler();

		HttpRequest*	getRequest() const;
		HttpResponse*	getResponse() const;
		Status			getLastStatus() const;
		void			setLastStatus(Status status);
};

void	handle_messages(const WebservConfig& config, const std::string &raw_request);

#endif //MESSAGEHANDLER_HPP