#include "console/console.hpp"
#include "server/server.hpp"
#include "config/WebservConfig.hpp"

const bool PRINTCONFIG = false;

int main(int ac, char **av) {
	if (ac != 2){
		console::log("Config file error ", ERROR, ALL);
		return 1;
	}
	std::string filename = av[1];
	std::string fn = "./config/" + filename;

	if (fn.find(".conf") == std::string::npos){
		fn += ".conf";
	}
	WebservConfig config;
	if (!config.loadConfig(fn)){
		console::log("config file error", ERROR, JR);
		console::log(fn, WARNING, JR);
		console::log("Detail: " + config.getLastError(), INFO, ALL);
		return 1;
	} 
	else {
		console::log("config loaded succeffulsy", INFO, ALL);
		if (PRINTCONFIG) config.printConfig();
	}
	std::string portStr = config.getDirective("listen");
	int port = atoi(portStr.c_str());
	server serv(port);
	while (1) {
		serv.serverManager(config);
	}
	return 0;
}

// int main() {
// 	console::openFile();
// 	console::log("coucou", SRV);
// 	console::log("coucou", SRV);
// 	console::log("coucou", MSG);
// 	console::log("coucou", ERROR);
// 	console::log("coucou", CONF);
// 	console::closeFile();
// }
