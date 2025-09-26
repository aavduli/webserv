#ifndef MESSAGEHANDLER_HPP
#define MESSAGEHANDLER_HPP

#include "../../errors/errors.hpp"
#include "../parsing/MessageParser.hpp"
#include "../parsing/RequestParser.hpp"
#include "../data/HttpMessage.hpp"
#include "../data/HttpRequest.hpp"
#include "../data/HttpResponse.hpp"
#include "../../config/WebservConfig.hpp"

 class MessageHandler {

	private:
		Error					_error;
		State					_state;
		HttpRequest*			_request;
		HttpResponse*			_response;

	public:
		MessageHandler(HttpRequest* request);
		MessageHandler(const MessageHandler& rhs);
		MessageHandler& operator=(const MessageHandler& rhs);
		~MessageHandler();

		HttpRequest*	getRequest() const;
		HttpResponse*	getResponse() const;
		
		bool			is_valid_request(const WebservConfig& config);
		void			process_request();
		void			generate_response();
		std::string		serialize_response();
		
		void			handle_get();
		void			handle_post();
		void			handle_delete();
		void			handle_head();
};

void	handle_request(const WebservConfig& config, const std::string &raw);

#endif //MESSAGEHANDLER_HPP