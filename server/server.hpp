#ifndef SERVER_HPP
# define SERVER_HPP
# include <iostream>
# include <string>
# include <fstream>

class server {
private:


public:
	server();
	server(const server &other);
	~server();

	server &operator=(const server &rhs);

};

#endif
