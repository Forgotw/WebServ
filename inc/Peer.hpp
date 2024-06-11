#pragma once

#include "Request.hpp"
#include "CgiProcess.hpp"
#include "Server.hpp"

#include <arpa/inet.h>
#include <fstream>
#include <sstream>

enum PeerState {
    EMPTY,
    CONNECTED,
    WAITING_READ,
    WAITING_WRITE,
    WAITING_CGI
};


class Peer {
public:

	Peer();
	~Peer();

	void				connect(int sockfd, struct sockaddr_in addr, Server* server);
	void				readRequest(void);
	void				writeResponse(void);
	void				handleHttpRequest(void);
    void                handleCgiProcess(void);

	void				handleCookies(std::string &request);
	

	/*-----Set-----*/
    void                setCgiProcess(CgiProcess* cgiprocess) { _cgiProcess = cgiprocess; }
    void                setStatus(PeerState state) { _status = state; }
	void				setRequest(const std::string& buffer);
	void				setReponse(std::string const &response);
	void				reset();

	/*-----Get-----*/
	int					getSocket() const { return this->_sockfd; }
	struct sockaddr_in	getAddr() const { return this->_addr; }
	int					getStatus() const { return this->_status; }
	const Request* 		getRequest() const { return this->_request; }
	const Server*		getServer() const { return this->_server; }
	const std::string&	getResponse() const { return this->_response; }
	CgiProcess*         getCgiProcess() { return this->_cgiProcess; }
    sessions            getSession() const { return this->_session; }
	const std::string&  getSessionId() const { return this->_session.sessionID; }
	time_t				getLastActivity() const { return this->_lastActivity; }


private:
	int					_sockfd;
	struct sockaddr_in	_addr;
	int					_status;
	Request*			_request;
    CgiProcess*         _cgiProcess;
	Server*				_server;
	std::string			_response;
	time_t				_lastActivity;

	std::string			_requestHeader;
	std::string			_requestBody;
	std::string			_requestMethod;
	std::string			_requestBoundary;
	std::string			_requestContentType;
	size_t				_requestContentLength;

	size_t				_responsePos;

	void getMethod();
	void getContentLength();
	void getContentType();
	void getBoundary();

	sessions			_session;
	std::string			_cookie;
};
