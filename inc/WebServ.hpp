#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "Server.hpp"
#include "Peer.hpp"

#include <sys/select.h>

#include <string>
#include <vector>
#include <queue>

class WebServ {
public:
	WebServ(std::vector<ServerConfig>& serverConfigVector);
	~WebServ();

	void start();
private:
	std::vector<Server *> _serverSockets;
	Peer _peers[FD_SETSIZE];
	fd_set _readfds;
	fd_set _writefds;
	fd_set _exceptfds;

	void startServers();

	void addServerToSet();
	void addPeerToReadSet();
	void addPeerToWriteSet();
	void addFdToExceptSet();
	void setupSets();

	void checkTimeout();

	void handleNewConnection();
	void handlePeerRequest();
	void handlePeerResponse();
	void handleExcept();
	void handleHttp();

};

#endif
