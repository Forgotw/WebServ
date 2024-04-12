#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <ostream>
#include "ServerConfig.hpp"

class Server {
public:
	Server(ServerConfig &config);
	~Server();

	void run();


	/*-----Get-----*/
	int getSocket() const { return _sockfd; }
	bool isRunning() const { return _isRunning; }
	const ServerConfig& getConfig() const { return _config; }


private:
	int				_sockfd;
	bool			_isRunning;
	ServerConfig	_config;
};
std::ostream &operator<<(std::ostream &os, Server const &ref);

#endif
