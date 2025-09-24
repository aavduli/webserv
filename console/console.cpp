#include "console.hpp"
#include <iostream>

std::ofstream console::_srv;
std::ofstream console::_msg;
std::ofstream console::_conf;
std::ofstream console::_error;

void console::openFile()  {
	_srv.open("logs/server.log");
	_msg.open("logs/msg.log");
	_conf.open("logs/conf.log");
	_error.open("logs/error.log");
}

void console::closeFile() {
	_srv.close();
	_msg.close();
	_conf.close();
	_error.close();
}

void console::log(std::string log, Typelog type) {
	if (type == SRV)
		_srv << "[SRV] " + log + "\n";
	else if (type == MSG)
		_msg << "[MSG] " + log + "\n";
	else if (type == CONF)
		_conf << "[CONF] " + log + "\n";
	else if (type == ERROR)
		_error << "[ERROR] " + log + "\n";
}
