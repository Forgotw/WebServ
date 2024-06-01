#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <ostream>
#include "ServerConfig.hpp"
#include "Request.hpp"


class Server {
public:
	Server(ServerConfig &config);
	~Server();

	void run();


	/*-----Get-----*/
	int					getSocket() const { return _sockfd; }
	bool				isRunning() const { return _isRunning; }
	const ServerConfig&	getConfig() const { return _config; }
	const Location*		findCgiLocation(const std::string& path) const;
	const Location*		findLocation(std::string path) const;
	std::string			findRequestedPath(const Location* location, std::string path) const;
	unsigned int		generateResponseCode(const Location* location, const Location* cgiLocation, std::string realPath, const Request& request) const;
	std::string			generateReponseFilePath(unsigned int code, std::string realPath) const;
	std::string			ServerHandleCGI(unsigned int& code, const Location* foundLocation, const std::string& cgiFilePath, const Request& request);
	std::string			ResponseRouter(const Request& request) const;
private:
	int				_sockfd;
	bool			_isRunning;
	ServerConfig	_config;

};
std::ostream &operator<<(std::ostream &os, Server const &ref);
unsigned int checkCgiError(const std::string& cgiBin, const std::string& realPath);

#endif
