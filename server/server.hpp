#ifndef SERVER_HPP
# define SERVER_HPP

# include "NetworkHandler.hpp"
# include "ConnectionManager.hpp"
# include "EventProcessor.hpp"
# include "../config/WebservConfig.hpp"
# include <csignal>

class server {
	private:
		int _port;
		static int _shutdown_requested;
		static struct sigaction _sa;
	public:
		server(int port);
		~server();

		static void signalHandler(int signal);
		static void setupSignalHandler();
		void serverManager(WebservConfig &config);
		int getPort() const;
		static int getShutDownRequest();
		static void setShutDownRequest();
};

#endif
