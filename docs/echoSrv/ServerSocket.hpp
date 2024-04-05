#ifndef SERVERSOCKET_HPP
#define SERVERSOCKET_HPP

#include <string>
#include <ostream>

class ServerSocket {
public:
	ServerSocket(int port);
	ServerSocket(std::string const &port, std::string const &ip);
	~ServerSocket();

	void run();

	int getSocket() const;
	std::string const &getIP() const;
	int getPort() const;
	bool isRunning() const;
private:
	int _sockfd;
	std::string _ip;
	int _port;
	bool _isRunning;
};
std::ostream &operator<<(std::ostream &os, ServerSocket const &ref);

#endif
