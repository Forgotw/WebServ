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

std::string generateSetCookieHeader(const std::string& sessionId) {
    std::stringstream ss;
    ss << "Set-Cookie: SESSIONID=" << sessionId << "; Path=/; HttpOnly";
    return ss.str();
}

void	Peer::handleCookies()
{
	std::string value = this->_request->getHeaders().find("Cookie")->second;
	std::string sessionID;
	std::string delimiter = "=";
	//std::cout << "value : " << value << std::endl;
	//std::cout << this->_request->getHeaders().find("Cookie")->second << std::endl;
	size_t pos = value.find(delimiter);
	if (pos != std::string::npos)
	{
		sessionID = value.substr(pos + delimiter.length());
		std::cout << "session reconnue :" << sessionID << std::endl;
	}
	if (!sessionID.empty()) {
        std::map<std::string, sessions>& sessionsMap = _server->getSessions();
        std::map<std::string, sessions>::iterator it = sessionsMap.find(sessionID);

        if (it != sessionsMap.end())
		{
            this->_session = &(it->second);
            std::cout << "Session trouvée pour l'ID : " << sessionID << std::endl;
        }
		else
		{
            this->_session = _server->newSession();
            std::cout << "Nouvelle session créée avec un nouvel ID." << std::endl;
        }
    }
	else
	{
        this->_session = _server->newSession();
		std::cout << _session->sessionID;
		_cookie = generateSetCookieHeader(this->_session->sessionID);
        std::cout << "Nouvelle session créée avec un nouvel ID." << std::endl;
    }
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
		this->_request->printRequest();
		// lire les cookies et set la session
		handleCookies();
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

void setCookie(std::string& response, std::string cookie)
{
        std::string delimiter = "\r\n";
        size_t pos = response.find(delimiter);

        if (pos != std::string::npos) {
            response.insert(pos + delimiter.length(), cookie + delimiter);
        }
    }

void	Peer::handleHttpRequest() {
			const Server		*server = getServer();
			const Request		request = *getRequest();
			std::string response;
			response = server->ResponseRouter(request);
			if (!_cookie.empty())
				setCookie(response, _cookie);
			//std::cout << response << std::endl;
			setReponse(response);
}
