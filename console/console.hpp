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
	ERROR,
	DEBUG
};

class console {
private:
	console();
	static bool _debug;

public:

	static void setDebug(bool enable);
	static void log(const std::string &msg, Level level = INFO);

};

#endif
