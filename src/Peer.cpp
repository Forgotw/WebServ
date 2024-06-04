#include "Peer.hpp"

#include <unistd.h>

#include <cstring>
#include <algorithm>

#include "Response.hpp"

Peer::Peer() {
	this->_status = EMPTY;
	this->_sockfd = 0;
	std::memset(&this->_addr, 0, sizeof(this->_addr));
	this->_request = NULL;
	this->_lastActivity = 0;
	_requestComplete = false;
	_headerComplete = false;
	_requestHeader = "";
	_requestBody = "";
	_requestMethod = "";
	_requestBoundary = "";
	_requestContentType = "";
	_requestContentLength = 0;
	_responsePos = 0;
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
void Peer::setRequest(const std::string& requestString) {
	this->_request = new Request(requestString);
	this->_status = WAITING_READ;
}

void Peer::setReponse(std::string const& response) {
	this->_response = response;
	this->_status = WAITING_WRITE;
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
	_requestHeader = "";
	_requestBody = "";
	_requestMethod = "";
	_requestBoundary = "";
	_requestContentType = "";
	_requestContentLength = 0;
	_responsePos = 0;
}

void Peer::readRequest() {
	try {
		std::string buffer;
		buffer.resize(8193);

		ssize_t byteRead = recv(getSocket(), &buffer[0], buffer.size() - 1, 0);
		if (byteRead == 0) {
			throw std::runtime_error("recv() - User disconnected");
		} else if (byteRead < 0) {
			throw std::runtime_error("recv() - Error during reading");
		}
		buffer.resize(byteRead);
		if (_requestHeader.empty()) {
			size_t posEndHeader = buffer.find("\r\n\r\n");
			if (posEndHeader != std::string::npos) {
				_requestHeader = buffer.substr(0, posEndHeader + 4);
				_requestBody = buffer.substr(posEndHeader + 4);
				getMethod();
				if (_requestMethod == "GET" || _requestMethod == "DELETE") {
					setRequest(_requestHeader);
					return;
				} else if (_requestMethod == "POST") {
					getContentLength();
					getContentType();
					getBoundary();
				} else {
					// TODO: Doit être une erreur HTTP (invalid method)
					throw std::runtime_error("readRequest() - Invalid method");
				}
			} else {
				// TODO: Dois être une erreur HTTP
				throw std::runtime_error("readRequest() - Header too long");
			}
		} else {
			_requestBody.append(buffer, 0, byteRead);
		}
		if (!_requestBoundary.empty()) {
			if (_requestBody.find(_requestBoundary + "--") != std::string::npos) {
				if (_requestBody.size() > _server->getConfig().getClientMaxBodySize()) {
					//TODO: il faut retourner une erreur 413
					throw std::runtime_error("413 - Body too large");
				}
				std::string request = _requestHeader + _requestBody;
				setRequest(request);
				return;
			}
		}
		if (_requestBody.size() >= _requestContentLength) {
			if (_requestBody.size() > _server->getConfig().getClientMaxBodySize()) {
				//TODO: il faut retourner une erreur 413
				throw std::runtime_error("413 - Body too large");
			}
			std::string request = _requestHeader + _requestBody;
			setRequest(request);
			return;
		}
	} catch (std::exception& err) {
		std::cerr << "[!] " << err.what() << std::endl;
		reset();
		return;
	}
}

void Peer::getMethod() {
	size_t pos = _requestHeader.find(" ");
	if (pos != std::string::npos) {
		_requestMethod = _requestHeader.substr(0, pos);
	} else {
		// TODO: Dois retourner une erreur HTML (invalid header)
		throw std::runtime_error("getMethod() - Invalid header");
	}
}

void Peer::getContentLength() {
	size_t posStart = _requestHeader.find("Content-Length: ");
	if (posStart != std::string::npos) {
		posStart += 16;
		size_t posEnd = _requestHeader.find("\r\n", posStart);
		if (posEnd != std::string::npos) {
			std::string contentLengthStr =
				_requestHeader.substr(posStart, posEnd - posStart);
			_requestContentLength = static_cast<size_t>(
				std::strtoul(contentLengthStr.c_str(), NULL, 10));
		} else {
			//TODO: Il faut retourner une erreur HTTP
			throw std::runtime_error(
				"getContentLength() - Content-Length bad format");
		}
	} else {
		// Il faut retourner une erreur HTTP
		throw std::runtime_error("getContentLength() - Missing Content-Length");
	}
}

void Peer::getContentType() {
	size_t posStart = _requestHeader.find("Content-Type: ");
	if (posStart != std::string::npos) {
		posStart += 14;
		size_t posEnd = _requestHeader.find("\r\n", posStart);
		if (posEnd != std::string::npos) {
			_requestContentType =
				_requestHeader.substr(posStart, posEnd - posStart);
		} else {
			//TODO: Il faut retourner une erreur HTTP
			throw std::runtime_error(
				"getContentType() - Content-Type bad format");
		}
	} else {
		//TODO: Il faut retourner une erreur HTTP
		throw std::runtime_error("Missing Content-Type");
	}
}

void Peer::getBoundary() {
	std::string boundary;
	size_t posStart = _requestContentType.find("boundary=");
	if (posStart != std::string::npos) {
		posStart += 9;
		boundary = _requestContentType.substr(posStart);
		_requestBoundary = boundary;
	}
}

void Peer::writeResponse() {
	try {
		size_t httpResponseLen = getResponse().size();
		ssize_t bytesToSend = std::min(httpResponseLen - _responsePos, (size_t)8196);
		ssize_t byteWritten = send(getSocket(), getResponse().c_str() + _responsePos, bytesToSend, 0);
		if (byteWritten <= 0) {
			//ERROR
			reset();
			return;
		}
		_responsePos += byteWritten;
		if (_responsePos >= httpResponseLen) {
			reset();
		}
	} catch (std::exception &err) {
		reset();
		std::cout << "[!] writeResponse() - " << err.what() << std::endl;
	}
}

/* void Peer::writeResponse() {
	ssize_t httpReponseLen = getResponse().size();
	ssize_t totalByteWritten = 0;

	while (totalByteWritten < httpReponseLen) {
		ssize_t byteWritten = send(getSocket(), getResponse().c_str() + totalByteWritten, httpReponseLen - totalByteWritten, 0);
		if (byteWritten <= 0) {
			reset();
			return;
		}
		totalByteWritten += byteWritten;
	}
	reset();
} */

void Peer::handleHttpRequest() {
	const Server* server = getServer();
	const Request request = *getRequest();
	std::string response;
	response = server->ResponseRouter(request);
	setReponse(response);
}
