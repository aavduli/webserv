#ifndef SERVER_HPP
# define SERVER_HPP

# include "NetworkHandler.hpp"
# include "ConnectionManager.hpp"
# include "EventProcessor.hpp"
# include "../config/WebservConfig.hpp"
class server {
	private:
		int _port;
	public:
		server(int port);
		~server();

		void serverManager(WebservConfig &config);
		int getPort() const;
};

#endif
