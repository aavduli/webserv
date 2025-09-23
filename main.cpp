#include "console/console.hpp"
#include "server/server.hpp"
#include "config/WebservConfig.hpp"

const bool PRINTCONFIG = false;
const bool DEBBUGPRINT = false;

int main(int ac, char **av) {
	console::setDebug(true);
	if (ac != 2){
		console::log("Config file error ", ERROR);
		return 1;
	}
	std::string filename = av[1];
	std::string fn = "./config/" + filename;

	if (fn.find(".conf") == std::string::npos){
		fn += ".conf";
	}
	WebservConfig config;
	if (!config.loadConfig(fn)){
		console::log("config file error", ERROR);
		console::log(fn, WARNING);
		console::log("Detail: " + config.getLastError(), INFO);
		return 1;
	}else{
		console::log("config loaded succeffulsy", INFO);
		if (PRINTCONFIG) config.printConfig();
		//exemple of usage
		if (DEBBUGPRINT){
			  std::cout << "\n=== INTEGRATION TESTS ===" << std::endl;

			// for MessageParser
			size_t maxContent = config.getMaxContentLength();
			std::cout << "Max Content Length: " << maxContent << std::endl;

			// for MessageHandler - error page
			std::string page404 = config.getErrorPage(404);
			std::string page403 = config.getErrorPage(403);
			std::string page500 = config.getErrorPage(500);
			std::cout << "Error 404 page: " << page404 << std::endl;
			std::cout << "Error 403 page: " << page403 << std::endl;
			std::cout << "Error 500 page: " << page500 << std::endl;

			// for the routing
			bool hasApi = config.hasLocation("/api");
			bool hasUploads = config.hasLocation("/uploads");
			std::cout << "Has /api location: " << (hasApi ? "yes" : "no") << std::endl;
			std::cout << "Has /uploads location: " << (hasUploads ? "yes" : "no") << std::endl;

			// basic srv config
			std::cout << "Server: " << config.getHost() << ":" << config.getPort() << std::endl;
			std::cout << "=========================\n" << std::endl;

		}

	}
	std::string portStr = config.getDirective("listen");
	int port = atoi(portStr.c_str());
	server serv(port);
	while (1) {
		serv.serverManager(config);
	}
	return 0;
}
