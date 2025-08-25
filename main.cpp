#include "console/console.hpp"

int main() {
	console::log("is working", INFO);
	console::log("is working", WARNING);
	console::log("is working", ERROR);
	console::setDebug(true);
	console::log("is working", DEBUG);

	return 0;
}
