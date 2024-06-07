#include "WebServ.hpp"
#include "Server.hpp"
#include "Peer.hpp"
#include "Response.hpp"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>

#include <string>
#include <cstring>
#include <cerrno>
#include <iostream>
#include <fstream>

bool _stop = true;

void handle_sigint(int sig) {
	(void)sig;
	_stop = true;
}

WebServ::WebServ(std::vector<ServerConfig>& serverConfigVector) {
	std::cout << "Creating WebServ objet.\n";
	_stop = true;
	for (std::vector<ServerConfig>::iterator it = serverConfigVector.begin(); it != serverConfigVector.end(); it++) {
		(*it).printServerConfig();
		if (it->isValidServerConfig()) {
			this->_serverSockets.push_back(new Server(*it));
		}
	}
}

WebServ::~WebServ() {
	std::vector<Server *>::iterator it = this->_serverSockets.begin();
	for (int i = 0; i < FD_SETSIZE; i++) {
		if (_peers[i].getStatus() != Peer::EMPTY) {
			_peers[i].reset();
		}
	}
	for (; it != this->_serverSockets.end(); it++) {
		delete *it;
	}
}


void WebServ::start() {
	startServers();
	int activity;
	_stop = false;
	signal(SIGINT, handle_sigint);

	for (;!_stop;) {
		setupSets();
		activity = select(FD_SETSIZE, &this->_readfds, &this->_writefds, NULL, NULL);
		if (activity < 0) {
			throw std::runtime_error(std::string("select: ") + std::strerror(errno));
		}
		if (activity > 0) {
			handleNewConnection();
			handlePeerRequest();
			handlePeerResponse();
			handleHttp();
		}
	}
}

void WebServ::startServers() {
	std::vector<Server *>::iterator it = this->_serverSockets.begin();
	for (; it != this->_serverSockets.end(); it++) {
		(*it)->run();
	}
}

void WebServ::addServerToSet() {
	std::vector<Server *>::iterator it = this->_serverSockets.begin();
	for (; it != this->_serverSockets.end(); it++) {
		FD_SET((*it)->getSocket(), &this->_readfds);
	}
}
void WebServ::addPeerToReadSet() {
	for (int i = 0; i < FD_SETSIZE; i++) {
		if (this->_peers[i].getStatus() == Peer::CONNECTED) {
			FD_SET(this->_peers[i].getSocket(), &this->_readfds);
		}
	}
}
void WebServ::addPeerToWriteSet() {
	for (int i = 0; i < FD_SETSIZE; i++) {
		if (this->_peers[i].getStatus() == Peer::WAITING_WRITE) {
			FD_SET(this->_peers[i].getSocket(), &this->_writefds);
		}
	}
}
void WebServ::setupSets() {
	FD_ZERO(&this->_readfds);
	FD_ZERO(&this->_writefds);
	addServerToSet();
	addPeerToReadSet();
	addPeerToWriteSet();
}

void WebServ::handleNewConnection() {
	std::vector<Server *>::iterator it = this->_serverSockets.begin();
	for (; it != this->_serverSockets.end(); it++) {
		if (FD_ISSET((*it)->getSocket(), &this->_readfds)) {
			int newSocket;
			struct sockaddr_in peerSocketAddr;
			int peerSocketAddrLen = sizeof(peerSocketAddr);
			if ((newSocket = accept((*it)->getSocket(), (struct sockaddr *) &peerSocketAddr, (socklen_t *) &peerSocketAddrLen)) < 0) {
				throw std::runtime_error(std::string("accept: ") + std::strerror(errno));
			}
			int i = 0;
			for (; i < FD_SETSIZE; i++) {
				if (this->_peers[i].getStatus() == Peer::EMPTY) {
					this->_peers[i].connect(newSocket, peerSocketAddr, *it);
					break;
				}
			}
			if (i >= FD_SETSIZE) {
				close(newSocket);
			}
		}
	}
}

void WebServ::handlePeerRequest() {
	for (size_t i = 0; i < FD_SETSIZE; i++) {
		if (FD_ISSET(this->_peers[i].getSocket(), &this->_readfds)) {
			_peers[i].readRequest();
		}
	}
}

void WebServ::handlePeerResponse() {
	for (size_t i = 0; i < FD_SETSIZE; i++) {
		if (FD_ISSET(this->_peers[i].getSocket(), &this->_writefds)) {
			_peers[i].writeResponse();
		}
	}
}

void WebServ::handleHttp() {
	for (size_t i = 0; i < FD_SETSIZE; i++) {
		if (this->_peers[i].getStatus() == Peer::WAITING_READ) {
			_peers[i].handleHttpRequest();
		}
	}
}
