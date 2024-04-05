#include "WebSrv.hpp"
#include "ServerSocket.hpp"

#include <iostream>

WebSrv::WebSrv(std::string const &ip, std::string const &port) {
	this->_serverSockets.push_back(new ServerSocket(ip, port));
}
WebSrv::~WebSrv() {
	std::vector<ServerSocket *>::iterator it = this->_serverSockets.begin();
	for (;it != this->_serverSockets.end(); it++) {
		delete *it;
	}
}
ServerSocket *WebSrv::operator[](size_t index) {
	return this->_serverSockets[index];
}

void WebSrv::addServer(std::string const &ip, std::string const &port) {
	this->_serverSockets.push_back(new ServerSocket(ip, port));
}
void WebSrv::startServers() {
	std::vector<ServerSocket *>::iterator it = this->_serverSockets.begin();
	for (; it != this->_serverSockets.end(); it++) {
		(*it)->run();
	}
}
void WebSrv::showServers() const {
	std::vector<ServerSocket *>::const_iterator it = this->_serverSockets.begin();
	for (; it != this->_serverSockets.end(); it++) {
		std::cout << *(*it) << std::endl;
	}
}
