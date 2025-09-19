#include "console.hpp"

bool console::_debug = false;

void console::setDebug(bool enable) {
	_debug = enable;
}

void console::log(const std::string &msg, Level level) {
	if (_debug == true) {
		switch(level) {
			case INFO:
				std::cout << GREEN << "[INFO] " << msg << RESET << std::endl;
				break;
			case WARNING:
				std::cout << YELLOW << "[WARNING] " << msg << RESET << std::endl;
				break;
			case ERROR:
				std::cerr << RED << "[ERROR] " << msg << RESET << std::endl;
				break;
			case DEBUG:
				std::cout << BLUE << "[DEBUG] " << msg << RESET <<std::endl;
		}
	}
}
