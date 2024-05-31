#include "Peer.hpp"
#include "Response.hpp"

#include <unistd.h>
#include <cstring>

Peer::Peer() {
	this->_status = EMPTY;
	this->_sockfd = 0;
	std::memset(&this->_addr, 0, sizeof(this->_addr));
	this->_request = NULL;
	this->_lastActivity = 0;
	_requestComplete = false;
	_headerComplete = false;
}

Peer::~Peer() {
	if (this->_request) {
		delete this->_request;
	}
}

void Peer::connect(int sockfd, struct sockaddr_in addr, Server* server) {
	if (this->_status == EMPTY) {
		this->_sockfd = sockfd;
		this->_addr = addr;
		this->_status = CONNECTED;
		this->_server = server;
		_requestComplete = false;
		_headerComplete = false;
	}
}

void	Peer::setRequest(const std::string& requestString) {
	// std::cout << "New request Data\n";
	this->_request = new Request(requestString);
	// _request->printRequest();
	this->_status = WAITING_READ;
}


void Peer::setReponse(std::string const &response) {
	this->_response = response;
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
	// this->_response.requestcode = 0;
	// this->_response.header.clear();
	// this->_response.body.clear();
	this->_response.clear();
	this->_lastActivity = 0;
}

void Peer::readRequest() {
	char buffer[4096];
	std::string requestData;
	size_t contentLength = 0;
	std::string::size_type headerEndPos;

	size_t maxBodySize = DEF_MAX_BODY_SIZE;

	while (!_requestComplete) {
		memset(buffer, 0, sizeof(buffer)); // Clear the buffer
		ssize_t bytesRead = recv(getSocket(), buffer, sizeof(buffer) - 1, MSG_DONTWAIT); // Leave space for null terminator

		if (bytesRead > 0) {
			requestData.append(buffer, bytesRead);
			if (!_headerComplete) {
				headerEndPos = requestData.find("\r\n\r\n");
				if (headerEndPos != std::string::npos) {
					_headerComplete = true;
					std::string::size_type contentLengthPos = requestData.find("Content-Length:");
					if (contentLengthPos != std::string::npos) {
						contentLength = atoi(requestData.substr(contentLengthPos + strlen("Content-Length:")).c_str());
						if (contentLength > maxBodySize) {
							reset();
							return;
						}
					}
				}
			}
			if (_headerComplete) {
				if (requestData.size() - (headerEndPos + 4) >= contentLength) {
					_requestComplete = true;
				}
			}
		}
		// } else if (bytesRead == 0) {
		// 	break;
		// } else {
		// 	_requestComplete = false;
		// 	break;
		// }
	}
	if (_requestComplete) {
		// std::cout << "------SUCCESS-------\n";
        // std::cout << requestData;
		// std::cout << "------SUCCESS--------\n";
		setRequest(requestData);
		setLastActivity();
		_requestComplete = false;
		_headerComplete = false;
	} else {
		std::cout << "------ERROR--------\n";
		reset();
	}
}

void Peer::writeResponse() {
	ssize_t httpReponseLen = getResponse().size();
	ssize_t totalByteWritten = 0;

	while (totalByteWritten < httpReponseLen) {
		ssize_t byteWritten = send(getSocket(), getResponse().c_str() + totalByteWritten, httpReponseLen - totalByteWritten, MSG_DONTWAIT);
		if (byteWritten <= 0) {
			reset();
			return;
		}
		totalByteWritten += byteWritten;
	}

	setLastActivity();
	reset();
}

void	Peer::handleHttpRequest() {
			const Server		*server = getServer();
			const Request		request = *getRequest();
			std::string response;
			response = server->ResponseRouter(request);
			setReponse(response);
}