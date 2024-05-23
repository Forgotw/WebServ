#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <ostream>
#include "ServerConfig.hpp"
#include "Request.hpp"

typedef struct s_sessions {
	std::string sessionID;
	std::string info;
	std::string username;
}				sessions;

class Server {
public:
	Server(ServerConfig &config);
	~Server();

	void 	run();
	sessions*	newSession();

	/*-----Set-----*/
	void addSession(const std::string& sessionID, const sessions& session) {
        _sessions[sessionID] = session;}


	/*-----Get-----*/
	int								getSocket() const { return _sockfd; }
	bool							isRunning() const { return _isRunning; }
	const ServerConfig&				getConfig() const { return _config; }
	const Location*					findCgiLocation(const std::string& path) const;
	const Location*					findLocation(std::string path) const;
	std::string						findRequestedPath(const Location* location, std::string path) const;
	unsigned int					generateResponseCode(const Location* location, std::string realPath, const Request& request) const;
	std::string						generateReponseFilePath(unsigned int code, std::string realPath) const;
	std::string						ServerHandleCGI(unsigned int& code, const Location* foundLocation, const std::string& cgiFilePath, const Request& request);
	std::string						ResponseRouter(const Request& request) const;
	std::map<std::string, sessions>& getSessions() {
        return _sessions;
    }

private:
	int								_sockfd;
	bool							_isRunning;
	ServerConfig					_config;
	std::map<std::string, sessions> _sessions;

};
std::ostream &operator<<(std::ostream &os, Server const &ref);
unsigned int checkCgiError(const Location* location, std::string realPath, const Request& request);

#endif
