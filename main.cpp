#include "console/console.hpp"
#include "server/server.hpp"

int main() {
	console::setDebug(true);
	while (1)
		server serv(8080);
	return 0;
}
