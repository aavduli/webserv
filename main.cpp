#include "console/console.hpp"
#include "server/server.hpp"
#include "config/webserv_config.hpp"

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
		console::log("Config file error ", ERROR);
	}
	std::string portStr = config.getDirective("listen");
	int port = atoi(portStr.c_str());
	server serv(port);
	while (1) {
		serv.serverManager();
	}
	return 0;
}
