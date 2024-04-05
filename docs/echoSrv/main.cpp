#include "WebSrv.hpp"

#include <iostream>
#include <ios>

int main() {
	try {
		WebSrv websrv("127.0.0.1", "4242");
		websrv.addServer("127.0.0.1", "2121");
		websrv.addServer("127.0.0.1", "8080");
		websrv.addServer("127.0.0.1", "8081");
		websrv.start();
	} catch (std::exception &err) {
		std::cout << err.what() << std::endl;
	}
	return 0;
}
