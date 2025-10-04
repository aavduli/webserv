#ifndef MESSAGEHANDLER_HPP
#define MESSAGEHANDLER_HPP

#include "../../errors/errors.hpp"
#include "../parsing/MessageParser.hpp"
#include "../parsing/RequestParser.hpp"
#include "../data/HttpMessage.hpp"
#include "../data/HttpRequest.hpp"
#include "../data/HttpResponse.hpp"
#include "../../config/WebservConfig.hpp"

class MessageValidator;

class MessageHandler {

	private:
		Status					_status;
		HttpRequest*			_request;
		HttpResponse*			_response;

	public:
		MessageHandler(HttpRequest* request);
		MessageHandler(const MessageHandler& rhs);
		MessageHandler& operator=(const MessageHandler& rhs);
		~MessageHandler();

		HttpRequest*	getRequest() const;
		HttpResponse*	getResponse() const;
		Status			getStatus() const;
		
		void			processRequest();
		void			generateResponse();
		std::string		serializeResponse();
		
		void			handleGet();
		void			handlePost();
		void			handleDelete();
};

void	handle_request(const WebservConfig& config, const std::string &raw);

#endif //MESSAGEHANDLER_HPP