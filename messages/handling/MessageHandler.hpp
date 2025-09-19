#ifndef MESSAGEHANDLER_HPP
#define MESSAGEHANDLER_HPP

#include "../../errors/errors.hpp"
#include "../parsing/MessageParser.hpp"
#include "../data/HttpMessage.hpp"
#include "../data/HttpRequest.hpp"
#include "../data/HttpResponse.hpp"

/* 
GET: Retrieves a resource from the server.
 */
 class MessageHandler {

	private:
		Error				_error;
		State				_state;
		HttpRequest*		_request;	// make const
		HttpResponse*		_response;

	public:
		MessageHandler(HttpRequest* request);
		MessageHandler(const MessageHandler& rhs);
		MessageHandler& operator=(const MessageHandler& rhs);
		~MessageHandler();

		HttpRequest*	getRequest() const;
		HttpResponse*	getResponse() const;

		bool		is_valid_request() const;
		void		process_request();
		void		generate_response();
		std::string	serialize_response();

		void		handle_get();
		void		handle_post();
		void		handle_delete();
		void		handle_head();
};

#endif //MESSAGEHANDLER_HPP