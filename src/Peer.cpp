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

void	Peer::readRequest() {
	char buffer[1024];
	std::string			requestData;
	size_t contentLength = 0;
	while (!_requestComplete) {
		ssize_t bytesRead = recv(getSocket(), buffer, sizeof(buffer), MSG_DONTWAIT);
		if (bytesRead > 0) {
			requestData.append(buffer, bytesRead);
			if (!_headerComplete) {
				if (strstr(&requestData[0], "\r\n\r\n")) {
					// std::cout << "Header Complete------------\n";
					_headerComplete = true;
					char* contentLengthPtr = strstr(&requestData[0], "Content-Length:");
					if (contentLengthPtr) {
						contentLength = atoi(contentLengthPtr + strlen("Content-Length:"));
					}
				}
			}
			if (_headerComplete) {
				// std::cout << "Checking if _requestComplete-------------\n";
				if (requestData.size() - (strstr(&requestData[0], "\r\n\r\n") - &requestData[0]) >= contentLength) {
					_requestComplete = true;
				}
			}
		} else if (bytesRead == 0 || (bytesRead < 0 && (errno == EWOULDBLOCK || errno == EAGAIN))) {
			break;
		} else if (bytesRead < 0) {
			throw std::runtime_error(std::string("recv: ") + std::strerror(errno));
		}
	}
	if (_requestComplete) {
		setRequest(requestData);
		setLastActivity();
		_requestComplete = false;
		_headerComplete = false;
	} else {
		reset();
	}
}

void	Peer::writeResponse() {
	ssize_t httpReponseLen = getResponse().size();
	ssize_t totalByteWritten = 0;

	for (;;) {
		ssize_t byteWritten = send(getSocket(), getResponse().c_str() + totalByteWritten, httpReponseLen - totalByteWritten, MSG_DONTWAIT);
		if (byteWritten < 0) {
			if (errno == EWOULDBLOCK || errno == EAGAIN) {
				continue;
			}
			throw std::runtime_error(std::string("send: ") + std::strerror(errno));
		} else {
			totalByteWritten += byteWritten;
			if (totalByteWritten >= httpReponseLen) {
				setLastActivity();
				reset();
				break;
			}
		}
	}
}

void	Peer::handleHttpRequest() {
			const Server		*server = getServer();
			const Request		request = *getRequest();
			request.printRequest();
			const Location*		foundLocation = server->findLocation(request.getURI().path);
			std::string			realPath = server->findRequestedPath(foundLocation, request.getURI().path);
			std::cout << "realPath Before1: " << realPath << "\n";
			unsigned int		responseCode = server->generateResponseCode(foundLocation, realPath, request);
			std::string			responseFilePath = server->generateReponseFilePath(responseCode, realPath);
			std::cout << "realPath: " << realPath << " responseCode: " << responseCode << " " << " responseFilePath: " << responseFilePath << std::endl;
			Response			response(foundLocation, responseFilePath, responseCode, request, &server->getConfig());
			setReponse(response.getResponse());
			std::cout << "End of handleHTTPRequest\n";
}
