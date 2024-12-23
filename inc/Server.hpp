#pragma once

#include <string>
#include <ostream>
#include "ServerConfig.hpp"
#include "Request.hpp"

typedef struct s_sessions {
	std::string sessionID;
	std::string info;
	std::string username;
}				sessions;

class Peer;

class Server {
public:
	Server(ServerConfig &config);
	~Server();

	void 	run();
	void	newSession(sessions& session);

	/*-----Set-----*/
	// void addSession(const std::string& sessionID, const sessions& session) {
    //     _sessions[sessionID] = session;}


	/*-----Get-----*/
	int					getSocket() const { return _sockfd; }
	bool				isRunning() const { return _isRunning; }
	const ServerConfig&	getConfig() const { return _config; }
	const Location*		findCgiLocation(const std::string& path) const;
	const Location*		findLocation(std::string path) const;
	std::string			findRequestedPath(const Location* location, std::string path) const;
	unsigned int		generateResponseCode(const Location* location, const Location* cgiLocation, std::string realPath, const Request& request) const;
	static std::string	generateReponseFilePath(unsigned int code, std::string realPath, const ServerConfig& config);
	std::string			ServerHandleCGI(unsigned int& code, const Location* foundLocation, const std::string& cgiFilePath, const Request& request);
	std::string			ResponseRouter(Peer& peer) const;
  std::vector<sessions>& getSessions() { return _sessions; }
private:
	int								_sockfd;
	bool							_isRunning;
	ServerConfig					_config;
	std::vector<sessions> 			_sessions;

};
std::ostream &operator<<(std::ostream &os, Server const &ref);
unsigned int checkCgiError(const std::string& cgiBin, const std::string& realPath);

