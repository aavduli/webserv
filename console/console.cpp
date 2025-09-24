#include "console.hpp"
#include <iostream>

std::ofstream console::_srv;
std::ofstream console::_msg;
std::ofstream console::_conf;
std::ofstream console::_error;

void console::openFile()  {
	_srv.open("logs/server.log", std::ios::out | std::ios::app);
	_msg.open("logs/msg.log", std::ios::out | std::ios::app);
	_conf.open("logs/conf.log", std::ios::out | std::ios::app);
	_error.open("logs/error.log", std::ios::out | std::ios::app);
	if (!_srv) std::cerr << "[console] Failed to open logs/server.log" << std::endl;
	if (!_msg) std::cerr << "[console] Failed to open logs/msg.log" << std::endl;
	if (!_conf) std::cerr << "[console] Failed to open logs/conf.log" << std::endl;
	if (!_error) std::cerr << "[console] Failed to open logs/error.log" << std::endl;
}

void console::closeFile() {
	_srv.close();
	_msg.close();
	_conf.close();
	_error.close();
}

void console::log(const std::string& log, Typelog type) {
	if (type == SRV) {
		_srv << "[SRV] " << log << std::endl;
	} else if (type == MSG) {
		_msg << "[MSG] " << log << std::endl;
	} else if (type == CONF) {
		_conf << "[CONF] " << log << std::endl;
	} else if (type == ERROR) {
		_error << "[ERROR] " << log << std::endl;
	}
	else
		std::cerr << "[console] Unknown log type" << std::endl;
}

void console::log(const std::string& log, const char* strerror, Typelog type) {
	if (type == SRV) {
		_srv << "[SRV] " << log << ": " << strerror << std::endl;
	} else if (type == MSG) {
		_msg << "[MSG] " << log << ": " << strerror << std::endl;
	} else if (type == CONF) {
		_conf << "[CONF] " << log << ": " << strerror << std::endl;
	} else if (type == ERROR) {
		_error << "[ERROR] " << log << ": " << strerror << std::endl;
	}
	else
		std::cerr << "[console] Unknown log type" << std::endl;
}

void console::log(const std::string& log, int nbr, Typelog type) {
	if (type == SRV) {
		_srv << "[SRV] " << log << " (" << nbr << ")" << std::endl;
	} else if (type == MSG) {
		_msg << "[MSG] " << log << " (" << nbr << ")" << std::endl;
	} else if (type == CONF) {
		_conf << "[CONF] " << log << " (" << nbr << ")" << std::endl;
	} else if (type == ERROR) {
		_error << "[ERROR] " << log << " (" << nbr << ")" << std::endl;
	}
	else
		std::cerr << "[console] Unknown log type" << std::endl;
}
