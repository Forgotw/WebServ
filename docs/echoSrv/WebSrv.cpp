#include "WebSrv.hpp"
#include "ServerSocket.hpp"

#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

WebSrv::WebSrv(std::string const &ip, std::string const &port) {
	for (int i = 0; i < FD_SETSIZE; i++) {
		_peerSockets[i] = 0;
	}
	this->_serverSockets.push_back(new ServerSocket(ip, port));
}
WebSrv::~WebSrv() {
	std::vector<ServerSocket *>::iterator it = this->_serverSockets.begin();
	for (;it != this->_serverSockets.end(); it++) {
		delete *it;
	}
	for (int i = 0; i < FD_SETSIZE; i++) {
		if (this->_peerSockets[i] > 0) {
			close (this->_peerSockets[i]);
		}
	}
}
ServerSocket *WebSrv::operator[](size_t index) {
	return this->_serverSockets[index];
}

void WebSrv::start() {
	this->startServers();
	int activity;
	for (;;) {
		FD_ZERO(&this->_readfds);
		addServerToReadSet();
		addPeerToReadSet();
		activity = select(FD_SETSIZE, &this->_readfds, NULL, NULL, 0);
		if (activity < 0) {
			throw std::runtime_error("select");
		}
		if (activity > 0) {
			handleNewConnection();
			handlePeerRequest();
		}
	}
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

void WebSrv::addServerToReadSet() {
	std::vector<ServerSocket *>::iterator it = this->_serverSockets.begin();
	for (; it != this->_serverSockets.end(); it++) {
		FD_SET((*it)->getSocket(), &this->_readfds);
	}
}
void WebSrv::addPeerToReadSet() {
	for (int i = 0; i < FD_SETSIZE; i++) {
		if (this->_peerSockets[i] > 0) {
			FD_SET(this->_peerSockets[i], &this->_readfds);
		}
	}
}

void WebSrv::handleNewConnection() {
	std::vector<ServerSocket *>::iterator it = this->_serverSockets.begin();
	for (; it != this->_serverSockets.end(); it++) {
		if (FD_ISSET((*it)->getSocket(), &this->_readfds)) {
			int newSocket;
			struct sockaddr_in peerSocketAddr;
			int peerSocketAddrLen = sizeof(peerSocketAddr);
			if ((newSocket = accept((*it)->getSocket(), (struct sockaddr *) &peerSocketAddr, (socklen_t *) &peerSocketAddrLen)) < 0) {
				throw std::runtime_error("accept");
			}
			std::cout << "New connection !" << std::endl;
			for (int i = 0; i < FD_SETSIZE; i++) {
				if (this->_peerSockets[i] == 0) {
					this->_peerSockets[i] = newSocket;
					break;
				}
			}
		}
	}
}
void WebSrv::handlePeerRequest() {
	ssize_t bytesRead;
	char buffer[1024];

	for (int i = 0; i < FD_SETSIZE; i++) {
		if (FD_ISSET(this->_peerSockets[i], &this->_readfds)) {
			bytesRead = read(this->_peerSockets[i], buffer, 1024);
			if (bytesRead < 0) {
				if (errno == EWOULDBLOCK || errno == EAGAIN) {
					continue;
				}
				std::runtime_error("read");
			} else if (bytesRead == 0) {
				std::cout << "Disconnect" << std::endl;
				close(this->_peerSockets[i]);
				this->_peerSockets[i] = 0;
			} else {
				buffer[bytesRead] = '\0';
				if (std::strncmp(buffer, "exit --force\n", bytesRead) == 0) {
					throw std::runtime_error("quit");
				}
				int byteWritten = write(this->_peerSockets[i], buffer, std::strlen(buffer));
				if (byteWritten < 0) {
					if (errno == EWOULDBLOCK || errno == EAGAIN) {
						continue;
					}
					throw std::runtime_error("write");
				}
			}
		}
	}
}
