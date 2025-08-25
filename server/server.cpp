#include "server.hpp"

server::server() {}


server::server(const server &other) : { //copy constructor
}

server::~server() {}

server &server::operator=(const server &rhs) {
	if (this != &rhs) {}
	return *this;
}

std::ostream& operator<<(std::ostream& os, const server &b) {
	// os << b. << " " << b. << ".";
	// return os;
}

