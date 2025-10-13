#include "console/console.hpp"
#include "server/server.hpp"
#include "config/WebservConfig.hpp"

const bool PRINTCONFIG = false;

int main(int ac, char **av) {
	console::openFile();
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
		console::log(fn, CONF);
		console::log("Detail: " + config.getLastError(), ERROR);
		return 1;
	} 
	else {
		console::log("config loaded succeffulsy", CONF);
		if (PRINTCONFIG) config.printConfig();

		// int port = config.getPort();
		// std::string hsot = config.getHost();
		// size_t maxbodysize = config.getMaxBodySize();
		// std::vector<std::string> allowedMethods = config.getAllowedMethods();
		// std::string serverName = config.getServerName();
		// std::string root = config.getRoot();
		// std::string index = config.getIndex();
	}
	std::string portStr = config.getDirective("listen");
	int port = atoi(portStr.c_str());
	server serv(port);
	serv.serverManager(config);
	console::closeFile();
	return 0;
}
