#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <ostream>

class Server {
public:
	Server(std::string const &ip, std::string const &port);
	~Server();

	void run();

	int getSocket() const;
	std::string const &getIP() const;
	int getPort() const;
	bool isRunning() const;

private:
	int			_sockfd;
	std::string	_ip;
	int			_port;
	bool		_isRunning;
};
std::ostream &operator<<(std::ostream &os, Server const &ref);

#endif
