#include "ServerSocket.hpp"

#include <iostream>
#include <ios>
#include <vector>

int main() {

	std::vector<ServerSocket> serverSockets;

	serverSockets.push_back(ServerSocket("127.0.0.1", "4242"));
	serverSockets.push_back(ServerSocket("127.0.0.1", "2121"));

	serverSockets[0].run();
	serverSockets[1].run();
	return 0;
}
