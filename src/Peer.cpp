#include "Peer.hpp"

#include <arpa/inet.h>
#include <unistd.h>

#include <cstring>

Peer::Peer() {
	this->_status = EMPTY;
	this->_sockfd = 0;
	std::memset(&this->_addr, 0, sizeof(this->_addr));
	this->_request = NULL;
	this->_lastActivity = 0;
}
Peer::~Peer() {
	if (this->_request) {
		delete this->_request;
	}
}

void Peer::connect(int sockfd, struct sockaddr_in addr) {
	if (this->_status == EMPTY) {
		this->_sockfd = sockfd;
		this->_addr = addr;
		this->_status = CONNECTED;
	}
}
void Peer::setRequest(std::string const &buffer) {
	this->_request = new Request(buffer);
	this->_status = WAITING_READ;
}
void Peer::setReponse(std::string const &response) {
	this->_response = response.c_str();
	this->_status = WAITING_WRITE;
}
void Peer::setLastActivity() {
	this->_lastActivity = time(NULL);
}
void Peer::reset() {
	close(this->_sockfd);
	this->_sockfd = 0;
	this->_status = EMPTY;
	std::memset(&this->_addr, 0, sizeof(this->_addr));
	delete this->_request;
	this->_request = NULL;
	this->_response.clear();
	this->_lastActivity = 0;
}

int Peer::getSocket() const { return this->_sockfd; }
struct sockaddr_in Peer::getAddr() const { return this->_addr; }
int Peer::getStatus() const { return this->_status; }
Request const *Peer::getRequest() const { return this->_request; }
std::string const &Peer::getResponse() const { return this->_response; }
time_t Peer::getLastActivity() const { return this->_lastActivity; }
