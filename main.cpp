#include "console/console.hpp"
#include "server/server.hpp"
#include "config/WebservConfig.hpp"

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
		//config.printConfig();
	}



	server serv(8080);
	while (1) {
		serv.serverManager();
	}
	return 0;
}
