#include "WebServ.hpp"

#include <iostream>

int main() {
	try {
		WebServ master("127.0.0.1", "8080");
		master.start();
	} catch (std::exception &err) {
		std::cerr << "[!] " << err.what() << std::endl;
	}
	return 0;
}
