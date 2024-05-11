#include "WebServ.hpp"
#include "Server.hpp"
#include "Peer.hpp"
#include "Response.hpp"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <string>
#include <cstring>
#include <cerrno>
#include <iostream>
#include <fstream>


#define TIMEOUT 3

WebServ::WebServ(std::vector<ServerConfig>& serverConfigVector) {
	std::cout << "Creating WebServ objet.\n";
	for (std::vector<ServerConfig>::iterator it = serverConfigVector.begin(); it != serverConfigVector.end(); it++) {
		(*it).printServerConfig();
		this->_serverSockets.push_back(new Server(*it));
	}
}

WebServ::~WebServ() {
	std::vector<Server *>::iterator it = this->_serverSockets.begin();
	for (; it != this->_serverSockets.end(); it++) {
		delete *it;
	}
}

void WebServ::start() {
	startServers();
	int activity;
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 300;
	for (;;) {
		checkTimeout();
		setupSets();
		activity = select(FD_SETSIZE, &this->_readfds, &this->_writefds, &this->_exceptfds, &timeout);
		if (activity < 0) {
			throw std::runtime_error(std::string("select: ") + std::strerror(errno));
		}
		if (activity > 0) {
			handleExcept();
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
		FD_SET((*it)->getSocket(), &this->_exceptfds);
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
void WebServ::addFdToExceptSet() {
	for (int i = 0; i < FD_SETSIZE; i++) {
		if (this->_peers[i].getStatus() != Peer::EMPTY) {
			FD_SET(this->_peers[i].getSocket(), &this->_exceptfds);
		}
	}
}
void WebServ::setupSets() {
	FD_ZERO(&this->_readfds);
	FD_ZERO(&this->_writefds);
	FD_ZERO(&this->_exceptfds);
	addServerToSet();
	addPeerToReadSet();
	addPeerToWriteSet();
	addFdToExceptSet();
}

void WebServ::checkTimeout() {
	for (int i = 0; i < FD_SETSIZE; i++) {
		if (this->_peers[i].getStatus() != Peer::EMPTY && time(NULL) - this->_peers[i].getLastActivity() > TIMEOUT) {
			this->_peers[i].reset();
			std::cout << "TIMEOUT\n";
		}
	}
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
					this->_peers[i].setLastActivity();
					break;
				}
			}
			if (i == FD_SETSIZE) {
				std::cout << "Server full" << std::endl;
				close(newSocket);
			}
		}
	}
}
// void WebServ::handlePeerRequest() {
// 	char buffer[1024];
// 	for (size_t i = 0; i < FD_SETSIZE; i++) {
// 		if (FD_ISSET(this->_peers[i].getSocket(), &this->_readfds)) {
// 			std::string request;
// 			for (ssize_t byteRead = 1; byteRead > 0;) {
// 				byteRead = recv(this->_peers[i].getSocket(), buffer, 1024, MSG_DONTWAIT);
// 				if (byteRead > 0) {
// 					buffer[byteRead] = '\0';
// 					request += std::string(buffer);
// 				} else if (byteRead < 0 && errno != EWOULDBLOCK && errno != EAGAIN) {
// 					throw std::runtime_error(std::string("recv: ") + std::strerror(errno));
// 				}
// 			}
// 			if (!request.empty()) {
// 				this->_peers[i].setRequest(request);
// 				this->_peers[i].setLastActivity();
// 			} else {
// 				this->_peers[i].reset();
// 			}
// 		}
// 	}
// }

void WebServ::handlePeerRequest() {
	for (size_t i = 0; i < FD_SETSIZE; i++) {
		if (FD_ISSET(this->_peers[i].getSocket(), &this->_readfds)) {
			_peers[i].readRequest();
		}
	}
}

// void WebServ::handlePeerResponse() {
// 	for (size_t i = 0; i < FD_SETSIZE; i++) {
// 		if (FD_ISSET(this->_peers[i].getSocket(), &this->_writefds)) {
// 			// ssize_t httpReponseLen = std::strlen(this->_peers[i].getResponse().c_str());
// 			// const char	*response = _peers[i].getResponse().c_str();
// 			ssize_t httpReponseLen = _peers[i].getResponse().size();
// 			ssize_t totalByteWritten = 0;
// 			// std::cout << "\n\n------------Response-------------\n";
// 			// std::cout << response << "\n\n\n";
// 			// std::cout << "Size: " << httpReponseLen << std::endl;
// 			for (;;) {
// 				ssize_t byteWritten = send(this->_peers[i].getSocket(), this->_peers[i].getResponse().c_str() + totalByteWritten, httpReponseLen - totalByteWritten, MSG_DONTWAIT);
// 				if (byteWritten < 0) {
// 					if (errno == EWOULDBLOCK || errno == EAGAIN) {
// 						continue;
// 					}
// 					throw std::runtime_error(std::string("send: ") + std::strerror(errno));
// 				} else {
// 					totalByteWritten += byteWritten;
// 					if (totalByteWritten >= httpReponseLen) {
// 						this->_peers[i].setLastActivity();
// 						this->_peers[i].reset();
// 						break;
// 					}
// 				}
// 			}
// 		}
// 	}
// }
void WebServ::handlePeerResponse() {
	for (size_t i = 0; i < FD_SETSIZE; i++) {
		if (FD_ISSET(this->_peers[i].getSocket(), &this->_writefds)) {
			_peers[i].writeResponse();
		}
	}
}
void WebServ::handleExcept() {
	for (size_t i = 0; i < FD_SETSIZE; i++) {
		if (FD_ISSET(this->_peers[i].getSocket(), &this->_exceptfds)) {
			std::cout << "ERROR on: " << i << std::endl;
			this->_peers[i].reset();
		}
	}
}

void WebServ::handleHttp() {
	for (size_t i = 0; i < FD_SETSIZE; i++) {
		if (this->_peers[i].getStatus() == Peer::WAITING_READ) {
			const Server		*server = _peers[i].getServer();
			const Request		request = *(_peers[i].getRequest());
			const Location*		foundLocation = server->findLocation(request.getURI().path);
			request.printRequest();
			std::string			realPath = server->findRequestedPath(foundLocation, request.getURI().path);
			std::cout << "realPath Before: " << realPath << "\n";
			unsigned int		responseCode = server->generateResponseCode(foundLocation, realPath, request);
			std::string			responseFilePath = server->generateReponseFilePath(responseCode, realPath);
			std::cout << "realPath: " << realPath << " responseCode: " << responseCode << " " << " responseFilePath: " << responseFilePath << std::endl;
			Response			response(foundLocation, responseFilePath, responseCode, request, &server->getConfig());
			_peers[i].setReponse(response.getResponse());
		}
	}
}
