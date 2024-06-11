#include "Peer.hpp"

#include <unistd.h>

#include <algorithm>
#include <cstring>
#include <sstream>

#include "Response.hpp"
#include "CgiHandler.hpp"

Peer::Peer() {
	this->_status = EMPTY;
	this->_sockfd = 0;
	std::memset(&this->_addr, 0, sizeof(this->_addr));
	this->_request = NULL;
	this->_lastActivity = 0;
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

std::string generateSetCookieHeader(const std::string& sessionId) {
	std::stringstream ss;
	ss << "Set-Cookie: SESSIONID=" << sessionId << "; Path=/; HttpOnly";
	return ss.str();
}

void printSessions(const std::vector<sessions>& sessionsvec) {
	std::vector<sessions>::const_iterator it = sessionsvec.begin();
	while (it != sessionsvec.end()) {
		sessions session = *it;
		std::cout << "SessionID: " << session.sessionID << std::endl;
		std::cout << "Info: " << session.info << std::endl;
		std::cout << "Username: " << session.username << std::endl;
		std::cout << "------------------------" << std::endl;
		++it;
	}
}

std::string generateIncrementalString2() {
	static int counter = 0;
	std::stringstream ss;
	ss << counter;
	std::string result = ss.str();
	++counter;
	return result;
}

std::string generateRandomString2() {
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";
	static const int stringLength = 15;

	// Seed for random number generator
	static bool initialized = false;
	if (!initialized) {
		srand(static_cast<unsigned int>(time(0)));
		initialized = true;
	}

	std::string randomString;
	randomString.reserve(stringLength);

	for (int i = 0; i < stringLength; ++i) {
		randomString += alphanum[rand() % (sizeof(alphanum) - 1)];
	}

	return randomString;
}

std::string extractSessionID(const std::string& header) {
	std::istringstream stream(header);
	std::string line;

	while (std::getline(stream, line)) {
		if (line.find("Cookie") != std::string::npos) {
			size_t pos = line.find("SESSIONID=");
			if (pos != std::string::npos) {
				pos += 10;	// length of "SESSIONID="
				size_t end = line.find(";", pos);
				if (end == std::string::npos) {
					end = line.length();
				}
				// std::cout << "session existente :" << line.substr(pos, end -
				// pos) << std::endl;
				return line.substr(pos, end - pos - 1);
			}
		}
	}

	return "";
}

void Peer::handleCookies(std::string& request) {
	// std::string value = _request->getHeaders().find("Cookie")->second;
	std::string sessionID = extractSessionID(request);
	sessions session;
	if (!sessionID.empty()) {
		std::vector<sessions>& sessionsvec = _server->getSessions();
		std::vector<sessions>::iterator it = sessionsvec.begin();
		// std::cout << "sessions tab :\n";
		// printSessions(sessionsvec);
		// std::cout << "\n";
		while (it != sessionsvec.end() && (*it).sessionID != sessionID) it++;
		if (it != sessionsvec.end()) {
			this->_session = (*it);
			_cookie = generateSetCookieHeader(this->_session.sessionID);
			// std::cout << "Session trouvée pour l'ID : " << sessionID
			// 		  << std::endl;
		} else {
			// session.sessionID = generateIncrementalString2();
			session.sessionID = generateRandomString2();
			_server->newSession(session);
			this->_session = session;
			_cookie = generateSetCookieHeader(this->_session.sessionID);
			// std::cout << "Nouvelle session créée avec un nouvel ID remplace "
			// 			 "par l'ancien."
			// 		  << std::endl;
		}
	} else {
		// session.sessionID = generateIncrementalString2();
		session.sessionID = generateRandomString2();
		_server->newSession(session);
		this->_session = session;
		_cookie = generateSetCookieHeader(this->_session.sessionID);
		// std::cout << "Nouvelle session créée avec un nouvel ID." << std::endl;
	}
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
				if (_requestMethod.empty()) {
					setReponse(Response::earlyErrorResponse(_server, 400));
					return;
				}
				if (_requestMethod == "GET" || _requestMethod == "DELETE") {
					handleCookies(_requestHeader);
					setRequest(_requestHeader);
					return;
				} else if (_requestMethod == "POST") {
					getContentLength();
					if (_requestContentLength == 0) {
						setReponse(Response::earlyErrorResponse(_server, 411));
						return;
					}
					getContentType();
					getBoundary();
				} else {
					setReponse(Response::earlyErrorResponse(_server, 405));
					return;
				}
			} else {
				setReponse(Response::earlyErrorResponse(_server, 431));
				return;
			}
		} else {
			_requestBody.append(buffer, 0, byteRead);
		}
		if (!_requestBoundary.empty()) {
			if (_requestBody.find(_requestBoundary + "--") !=
				std::string::npos) {
				if (_requestBody.size() >
					_server->getConfig().getClientMaxBodySize()) {
					setReponse(Response::earlyErrorResponse(_server, 413));
					return;
				}
				std::string request = _requestHeader + _requestBody;
				handleCookies(request);
				setRequest(request);
				return;
			}
		}
		if (_requestBody.size() >= _requestContentLength) {
			if (_requestBody.size() >
				_server->getConfig().getClientMaxBodySize()) {
				setReponse(Response::earlyErrorResponse(_server, 413));
				return;
			}
			std::string request = _requestHeader + _requestBody;
			handleCookies(request);
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
		_requestMethod = "";
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
			_requestContentLength = 0;
		}
	} else {
		_requestContentLength = 0;
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
			// TODO: Il faut retourner une erreur HTTP
			throw std::runtime_error(
				"getContentType() - Content-Type bad format");
		}
	} else {
		// TODO: Il faut retourner une erreur HTTP
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
		ssize_t bytesToSend =
			std::min(httpResponseLen - _responsePos, (size_t)8196);
		ssize_t byteWritten = send(
			getSocket(), getResponse().c_str() + _responsePos, bytesToSend, 0);
		if (byteWritten <= 0) {
			// ERROR
			reset();
			return;
		}
		_responsePos += byteWritten;
		if (_responsePos >= httpResponseLen) {
			reset();
		}
	} catch (std::exception& err) {
		reset();
		std::cout << "[!] writeResponse() - " << err.what() << std::endl;
	}
}

void setCookie(std::string& response, std::string cookie) {
	std::string delimiter = "\r\n";
	size_t pos = response.find(delimiter);

	if (pos != std::string::npos) {
		response.insert(pos + delimiter.length(), cookie + delimiter);
	}
}

void Peer::handleHttpRequest() {
	const Server* server = getServer();
	const Request request = *getRequest();
	std::string response;
	response = server->ResponseRouter(*this);
	if (!_cookie.empty()) {
        setCookie(response, _cookie);
    }
    if (_status != WAITING_CGI) {
        setReponse(response);
    }
}

void    Peer::handleCgiProcess() {
    if (_cgiProcess == nullptr) {
        throw std::runtime_error(std::string("ERROR HANDLING CGI PROCESS"));
    }
    if (!_cgiProcess->isReadyToWrite()) {
        // std::cout << "_cgiProcess->checkProcessStatus();\n";
        _cgiProcess->checkProcessStatus();
        // std::cout << "_cgiProcess->checkProcessStatus(); end\n";
    } 
    if (_cgiProcess->isReadyToWrite() && !_cgiProcess->isReady()) {
        // std::cout << "_cgiProcess->writeCgiOuput();\n";
        _cgiProcess->writeCgiOuput();
        // std::cout << "_cgiProcess->writeCgiOuput(); end\n";
    }
    if (_cgiProcess->isReady()) {
        // std::cout << "setReponse(CgiHandler::ProcessCgiOutput(_server, _cgiProcess->getCgiOutputStr()));\n";
        setReponse(CgiHandler::ProcessCgiOutput(_server, _cgiProcess->getCgiOutputStr()));
        // std::cout << "setReponse(CgiHandler::ProcessCgiOutput(_server, _cgiProcess->getCgiOutputStr())); end\n";
        delete (_cgiProcess);
    }
}
