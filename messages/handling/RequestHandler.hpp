#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP

#include "MessageHandler.hpp"

class RequestHandler : public MessageHandler {

	public:
		RequestHandler(HttpRequest* request);
		RequestHandler(const RequestHandler& rhs);
		RequestHandler& operator=(const RequestHandler& rhs);
		~RequestHandler();

		void	processRequest();

	private:
		bool	validateGetRequest();
		bool	validatePostRequest();
		bool	validateDeleteRequest();
		void	processGetRequest();
		void	processPostRequest();
		void	processDeleteRequest();
};

#endif //REQUESTHANDLER_HPP