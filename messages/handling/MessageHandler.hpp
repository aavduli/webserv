#ifndef MESSAGEHANDLER_HPP
#define MESSAGEHANDLER_HPP

#include "../../errors/errors.hpp"
#include "../parsing/MessageParser.hpp"
#include "../MessageStreams.hpp"
#include "../data/HttpMessage.hpp"
#include "../data/HttpRequest.hpp"
#include "../data/HttpResponse.hpp"

 class MessageHandler {

	private:
		const HttpRequest*	_request;
		HttpResponse*		_response;

	public:
		MessageHandler(const HttpRequest* request);
		MessageHandler(const MessageHandler& rhs);
		MessageHandler& operator=(const MessageHandler& rhs);
		~MessageHandler();

		const HttpRequest*	getRequest() const;
		HttpResponse*		getResponse() const;

		bool		is_valid_request() const;
		void		process_request();
		void		generate_response();
		std::string	serialize_response();
};

#endif //MESSAGEHANDLER_HPP