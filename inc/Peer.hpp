#ifndef PEER_HPP
#define PEER_HPP

#include "Request.hpp"

#include <arpa/inet.h>

class Peer {
public:
	enum PeerState {
		EMPTY,
		CONNECTED,
		WAITING_READ,
		WAITING_WRITE
	};

	Peer();
	~Peer();

	void connect(int sockfd, struct sockaddr_in addr);
	void setRequest(std::string const &buffer);
	void setReponse(std::string const &response);
	void setLastActivity();
	void reset();

	int getSocket() const;
	struct sockaddr_in getAddr() const;
	int getStatus() const;
	Request const *getRequest() const;
	std::string const &getResponse() const;
	time_t getLastActivity() const;

private:
	int _sockfd;
	struct sockaddr_in _addr;
	int _status;
	Request *_request;
	std::string _response;
	time_t _lastActivity;
};

#endif
