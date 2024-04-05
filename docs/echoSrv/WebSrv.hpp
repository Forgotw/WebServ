#ifndef WEBSRV_HPP
#define WEBSRV_HPP

#include "ServerSocket.hpp"

#include <string>
#include <vector>

class WebSrv {
public:
	WebSrv(std::string const &ip, std::string const &port);
	~WebSrv();
	ServerSocket *operator[](size_t index);

	void addServer(std::string const &ip, std::string const &port);
	void startServers();
	void showServers() const;
private:
	std::vector<ServerSocket *> _serverSockets;
};

#endif
