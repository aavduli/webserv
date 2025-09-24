#ifndef CONSOLE_HPP
# define CONSOLE_HPP
# include <iostream>
# include <string>
# include <fstream>

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"

enum Level {
	INFO,
	WARNING,
	ERROR
};

enum Worker {
	AH,
	JR,
	AA,
	ALL
};

class console {
private:
	console();
public:
	static void log(const std::string& msg, Level level, Worker who);
};

#endif
