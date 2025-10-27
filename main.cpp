#include "console/console.hpp"
#include "server/server.hpp"
#include "config/WebservConfig.hpp"
#include "cgi/CgiExec.hpp"
#include <fstream>

int main(int ac, char **av) {

	console::openFile();
	std::string filename;
	if (ac == 2){
		filename = av[1];
	}else{
		filename = "default";
		console::log("No config file specified, using default", CONF);
	}

	std::string fn = "./configfiles/" + filename;

	if (fn.find(".conf") == std::string::npos){
		fn += ".conf";
	}
	WebservConfig config;
	console::clearLogFile(CONF);
	console::clearLogFile(ERROR);
	if (!config.loadConfig(fn)){
		console::log("config file error", ERROR);
		console::log("File: " + fn , ERROR);
		console::log("Detail: " + config.getLastError(), ERROR);

		std::ifstream test(fn.c_str());
		if (!test.good()){
			console::log("File does not exist: " + fn, ERROR);
		}
		test.close();
		console::closeFile();
		std::cout << RED << "Configuration error: " << config.getLastError()<< RESET << std::endl;
		return 1;
	}
	else{
		console::log("config loaded succeffulsy", CONF);
		config.printConfig();
	}
	std::vector<int> ports = config.getAllPorts();
	server serv(ports);
	serv.serverManager(config);
	console::closeFile();
	return 0;
}
