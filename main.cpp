#include "console/console.hpp"
#include "server/server.hpp"
#include "config/WebservConfig.hpp"
#include "cgi/CgiExec.hpp"

const bool PRINTCONFIG = false;

int testCGI(){
	HttpRequest request;
	request.setMethod("GET");

	RequestUri uri("/cgi_bin/hello.py?name=jim&age=42");
	uri.parse();
	request.setUri(uri);

	WebservConfig config;
	config.loadConfig("./config/test.conf");

	CgiExec executor("./www/cgi/script.py", "/usr/bin/python3", &config);
	std::string output = executor.execute(&request);

	std::cout << "=== CGI OUTPUT ===" << std::endl;
	std::cout << output << std::endl;
	std::cout << "================" << std::endl;

	return 0;

}

int main(int ac, char **av) {

	//testCGI();
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
	}
	std::string portStr = config.getDirective("listen");
	int port = atoi(portStr.c_str());
	server serv(port);
	serv.serverManager(config);
	console::closeFile();
	return 0;
}
