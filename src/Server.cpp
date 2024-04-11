#include "Server.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#include <string>
#include <cstring>
#include <sstream>
#include <iostream>
#include <ostream>
#include <cerrno>

#define LISTEN_BACKLOG 42

Server::Server(std::string const &ip, std::string const &port) {
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
		throw std::runtime_error(std::string("getaddrinfo: ") + gai_strerror(err));
	}

	for (rp = resp; rp != NULL; rp = rp->ai_next) {
		this->_sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		int enable = 1;
		if (setsockopt(this->_sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) == -1) {
			throw std::runtime_error(std::string("setsockopt: ") + std::strerror(errno));
		}
		if (this->_sockfd == -1) {
			continue;
		}
		if (bind(this->_sockfd, rp->ai_addr, rp->ai_addrlen) != -1) {
			break;
		}
		if (close(this->_sockfd) == -1) {
			throw std::runtime_error(std::string("close: ") + std::strerror(errno));
		}
	}
	freeaddrinfo(resp);
	if (rp == NULL) {
		throw std::runtime_error(std::string("Cannot create socket: ") + std::strerror(errno));
	}
	std::istringstream iss(port);
	iss >> this->_port;
	this->_ip = ip;
	this->_isRunning = false;
}
Server::~Server() {
	if (close(this->_sockfd) == -1) {
		throw std::runtime_error(std::string("close: ") + std::strerror(errno));
	}
	this->_isRunning = false;
	std::cout << "close socket: " << this->_sockfd << std::endl;
}

void Server::run() {
	if (!this->_isRunning) {
		if (listen(this->_sockfd, LISTEN_BACKLOG) == -1) {
			throw std::runtime_error(std::string("listen: ") + std::strerror(errno));
		}
		this->_isRunning = true;
		std::cout << "Server running on socket: " << this->_sockfd
				  << ", listening on: " << this->_ip << ":" << this->_port << std::endl;
	}
}

int Server::getSocket() const { return this->_sockfd; }
std::string const &Server::getIP() const { return this->_ip; }
int Server::getPort() const { return this->_port; }
bool Server::isRunning() const { return this->_isRunning; }

std::ostream &operator<<(std::ostream &os, Server const &ref) {
	os << "ServerSocket { socket: " << ref.getSocket()
	   << ", IP: " << ref.getIP()
	   << ", port: " << ref.getPort()
	   << ", running: " << std::boolalpha << ref.isRunning() << std::noboolalpha
	   << " }";
	return os;
}
