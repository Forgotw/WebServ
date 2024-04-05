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

	void start();

	void addServer(std::string const &ip, std::string const &port);
	void showServers() const;
private:
	std::vector<ServerSocket *> _serverSockets;
	fd_set _readfds;
	int _peerSockets[FD_SETSIZE];

	void startServers();
	void addServerToReadSet();
	void addPeerToReadSet();

	void handleNewConnection();
	void handlePeerRequest();
};

#endif
