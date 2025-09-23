
#include "console.hpp"
#include <iostream>

void console::log(const std::string& msg, Level level, Worker who) {
	switch (level) {
		case INFO:
			GREEN;
		case WARNING:
			YELLOW;
		case ERROR:
			RED;
		case DEBUG:
			BLUE;
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
	RESET;
}
