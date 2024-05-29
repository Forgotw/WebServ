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
	// delete this->_session;
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

// Définition de la fonction pour imprimer les sessions
void printSessions(const std::vector<sessions*>& sessionsvec) {
    std::vector<sessions*>::const_iterator it = sessionsvec.begin();
    while (it != sessionsvec.end()) {
        sessions* session = *it;
        std::cout << "SessionID: " << session->sessionID << std::endl;
        std::cout << "Info: " << session->info << std::endl;
        std::cout << "Username: " << session->username << std::endl;
        std::cout << "------------------------" << std::endl;
        ++it;
    }
}

// void	Peer::handleCookies()
// {
// 	std::string value = this->_request->getHeaders().find("Cookie")->second;
// 	std::string sessionID = "";
// 	std::string delimiter = "SESSIONID=";
// 	//std::cout << "value : " << value << std::endl;
// 	//std::cout << this->_request->getHeaders().find("Cookie")->second << std::endl;
// 	size_t pos = value.find(delimiter);
// 	if (pos != std::string::npos)
// 	{
// 		sessionID = value.substr(pos + delimiter.length());
// 		std::cout << "session reconnue :" << sessionID << std::endl;
// 	}

// 	sessions* session = nullptr;

//     if (!sessionID.empty()) {
//         std::vector<sessions>& sessionsvec = _server->getSessions();
//         for (std::vector<sessions>::iterator it = sessionsvec.begin(); it != sessionsvec.end(); ++it) {
//             if (it->sessionID == sessionID) {
//                 session = &(*it);
//                 break;
//             }
//         }
//     }

//     if (!session) {
//         sessions newSession = _server->newSession();
//         _server->getSessions().push_back(newSession);
//         session = &_server->getSessions().back();
//         std::string setCookieHeader = generateSetCookieHeader(session->sessionID);
//         std::cout << setCookieHeader << std::endl;
//     }

//     std::cout << "Session Info: " << session->info << std::endl;
// }

void	Peer::handleCookies()
{
	std::string value = this->_request->getHeaders().find("Cookie")->second;
	std::string sessionID = "";
	std::string delimiter = "SESSIONID=";
	//std::cout << "value : " << value << std::endl;
	//std::cout << this->_request->getHeaders().find("Cookie")->second << std::endl;
	size_t pos = value.find(delimiter);
	if (pos != std::string::npos)
	{
		sessionID = value.substr(pos + delimiter.length());
		std::cout << "session reconnue :" << sessionID << std::endl;
	}
	if (!sessionID.empty()) {
        // std::map<std::string, sessions>& sessionsMap = _server->getSessions();
        // std::map<std::string, sessions>::iterator it = sessionsMap.find(sessionID);
		std::vector<sessions>& sessionsvec = _server->getSessions();
		std::vector<sessions>::iterator it = sessionsvec.begin();
		// std::cout << "sessions tab :\n";
		// printSessions(sessionsvec);
		//std::cout << "\n";
		while (it != sessionsvec.end() && (*it).sessionID != sessionID)
			it++;
        if (it != sessionsvec.end())
		{
            this->_session = (*it);
			_cookie = generateSetCookieHeader(this->_session.sessionID);
            std::cout << "Session trouvée pour l'ID : " << sessionID << std::endl;
        }
		else
		{
            this->_session = _server->newSession();
			_cookie = generateSetCookieHeader(this->_session.sessionID);
            std::cout << "Nouvelle session créée avec un nouvel ID." << std::endl;
        }
    }
	else
	{
        this->_session = _server->newSession();
		std::cout << _session.sessionID;
		_cookie = generateSetCookieHeader(this->_session.sessionID);
        std::cout << "Nouvelle session créée avec un nouvel ID." << std::endl;
    }
}

std::string generateIncrementalString2() {
    static int counter = 0;
    std::string result = std::to_string(counter);
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

void	Peer::handleCookies2()
{
	std::string value = this->_request->getHeaders().find("Cookie")->second;
	std::string sessionID = "";
	std::string delimiter = "SESSIONID=";
	sessions session;
	//std::cout << "value : " << value << std::endl;
	//std::cout << this->_request->getHeaders().find("Cookie")->second << std::endl;
	size_t pos = value.find(delimiter);
	if (pos != std::string::npos)
	{
		sessionID = value.substr(pos + delimiter.length());
		std::cout << "session reconnue :" << sessionID << std::endl;
	}
	if (!sessionID.empty())
	{
		std::vector<sessions>& sessionsvec = _server->getSessions();
		std::vector<sessions>::iterator it = sessionsvec.begin();
		// std::cout << "sessions tab :\n";
		// printSessions(sessionsvec);
		//std::cout << "\n";
		while (it != sessionsvec.end() && (*it).sessionID != sessionID)
			it++;
        if (it != sessionsvec.end())
		{
            this->_session = (*it);
			_cookie = generateSetCookieHeader(this->_session.sessionID);
            std::cout << "Session trouvée pour l'ID : " << sessionID << std::endl;
        }
		else
		{
			session.sessionID = generateIncrementalString2();
			//session.sessionID = generateRandomString2();
            _server->newSession2(session);
			this->_session = session;
			_cookie = generateSetCookieHeader(this->_session.sessionID);
            std::cout << "Nouvelle session créée avec un nouvel ID." << std::endl;
        }
    }
	else
	{
        session.sessionID = generateIncrementalString2();
		//session.sessionID = generateRandomString2();
        _server->newSession2(session);
		this->_session = session;
		_cookie = generateSetCookieHeader(this->_session.sessionID);
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
		handleCookies2();
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
