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

std::string extractSessionID(const std::string& header) {
    std::istringstream stream(header);
    std::string line;

    while (std::getline(stream, line)) {
        if (line.find("Cookie") != std::string::npos) {
            size_t pos = line.find("SESSIONID=");
            if (pos != std::string::npos) {
                pos += 10; // length of "SESSIONID="
                size_t end = line.find(";", pos);
                if (end == std::string::npos) {
                    end = line.length();
                }
				//std::cout << "session existente :" << line.substr(pos, end - pos) << std::endl;
                return line.substr(pos, end - pos - 1);
            }
        }
    }

    return "";
}

void	Peer::handleCookies(std::string &request)
{
	//std::string value = _request->getHeaders().find("Cookie")->second;
	std::string sessionID = extractSessionID(request);
	sessions session;
	if (!sessionID.empty())
	{
		std::vector<sessions>& sessionsvec = _server->getSessions();
		std::vector<sessions>::iterator it = sessionsvec.begin();
		// std::cout << "sessions tab :\n";
		// printSessions(sessionsvec);
		// std::cout << "\n";
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
			// session.sessionID = generateIncrementalString2();
			session.sessionID = generateRandomString2();
            _server->newSession(session);
			this->_session = session;
			_cookie = generateSetCookieHeader(this->_session.sessionID);
            std::cout << "Nouvelle session créée avec un nouvel ID remplace par l'ancien." << std::endl;
        }
    }
	else
	{
        // session.sessionID = generateIncrementalString2();
		session.sessionID = generateRandomString2();
        _server->newSession(session);
		this->_session = session;
		_cookie = generateSetCookieHeader(this->_session.sessionID);
        std::cout << "Nouvelle session créée avec un nouvel ID." << std::endl;
    }
}

void	Peer::readRequest() {
	char buffer[1024];
	std::string			requestData;
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
                    // if (MaxbodysizeReached()) {
                    //     setResponse();
                    //     reset;
                    // }
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
		setRequest(requestData);
		handleCookies(requestData);
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