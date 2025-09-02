#include "console/console.hpp"
#include "server/server.hpp"

int main() {
	console::setDebug(true);
	server serv(8080);
	serv.startServer();

	return 0;
}
