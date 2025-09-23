
#include "console.hpp"
#include <iostream>

void console::log(const std::string& msg, Level level, Worker who) {
	switch (level) {
		case INFO:
			std::cout << GREEN;
			break;
		case WARNING:
			std::cout << YELLOW;
			break;
		case ERROR:
			std::cout << RED;
			break;
	}
	if (who == AA) {
		std::cout << "[SERVER] " << msg << std::endl;
	} else if (who == JR) {
		std::cout << "[CONFIG] " << msg << std::endl;
	} else if (who == AH) {
		std::cout << "[MESSAGES] " << msg << std::endl;
	} else if (who == ALL) {
		std::cout << "[ALL] " << msg << std::endl;
	}
	std::cout << RESET;
}
