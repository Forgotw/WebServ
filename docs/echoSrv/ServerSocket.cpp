#include "ServerSocket.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <string>
#include <cstring>
#include <stdexcept>
#include <iostream>

#define LISTEN_BACKLOG 42

ServerSocket::ServerSocket(int port) : _port(port) {}
ServerSocket::ServerSocket(std::string const &ip, std::string const &port) {
 	int err;
	struct addrinfo *resp;
	struct addrinfo *rp;
	struct addrinfo hints;
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	err = getaddrinfo(ip.c_str(), port.c_str(), &hints, &resp);
	if (err != 0) {
		throw std::runtime_error("getaddrinfo");
	}

	for (rp = resp; rp != NULL; rp = rp->ai_next) {
		this->_sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (this->_sockfd == -1) {
			continue;
		}
		if (bind(this->_sockfd, rp->ai_addr, rp->ai_addrlen) != -1) {
			break;
		}
		close(this->_sockfd);
	}
	freeaddrinfo(resp);
	if (rp == NULL) {
		throw std::runtime_error("Unable to make socket");
	}
	this->_ip = ip;
	this->_port = std::stoi(port);
	this->_isRunning = false;
}
ServerSocket::~ServerSocket() {
	std::cout << "close server" << std::endl;
	close(this->_sockfd);
}

void ServerSocket::run() {
	if (listen(this->_sockfd, LISTEN_BACKLOG) == -1) {
		throw std::runtime_error("listen");
	}
	this->_isRunning = true;
	std::cout << "Server running on socket: " << this->_sockfd
			  << ", listening on: " << this->_ip << ":" << this->_port << std::endl;
}

int ServerSocket::getSocket() const { return this->_sockfd; }
std::string const &ServerSocket::getIP() const { return this->_ip; }
int ServerSocket::getPort() const { return this->_port; }
bool ServerSocket::isRunning() const { return this->_isRunning; }

std::ostream &operator<<(std::ostream &os, ServerSocket const &ref) {
	os << "ServerSocket { socket: " << ref.getSocket()
	   << ", IP: " << ref.getIP()
	   << ", port: " << ref.getPort()
	   << ", running: " << std::boolalpha << ref.isRunning() << std::noboolalpha
	   << " }";
	return os;
}
