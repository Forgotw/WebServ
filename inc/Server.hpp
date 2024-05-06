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
	const Location*		findLocation(std::string path) const;
	std::string			findRequestedPath(const Location* location, std::string path) const;
	unsigned int		generateResponseCode(const Location* location, std::string realPath, const Request& request) const;
	std::string			generateReponseFilePath(unsigned int responseCode, std::string realPath) const;
	

private:
	int				_sockfd;
	bool			_isRunning;
	ServerConfig	_config;
};
std::ostream &operator<<(std::ostream &os, Server const &ref);

#endif
