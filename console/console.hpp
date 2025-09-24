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

enum Typelog {
	SRV,
	MSG,
	CONF,
	ERROR
};

class console {
private:
console();

public:
	static std::ofstream _srv;
	static std::ofstream _msg;
	static std::ofstream _conf;
	static std::ofstream _error;

	static void openFile();
	static void closeFile();

	static void log(std::string log, Typelog type);
};
#endif
