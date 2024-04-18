#ifndef PEER_HPP
#define PEER_HPP

#include "Request.hpp"
#include "Server.hpp"

#include <arpa/inet.h>
#include <fstream>
#include <sstream>

typedef struct 		s_response {
	unsigned int	requestcode;
	std::string		pathToRespFile;
	bool			list;
}					t_response;

std::string 		httpGetFormatter(unsigned int reqCode, std::string pathToFile);
void 				handleErrors(t_response *response);
std::string 		treatRequest(Request const *Request, Server const *serv);
void				findRequestLocation(t_response *response);


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

	void				connect(int sockfd, struct sockaddr_in addr, Server* server);
	unsigned int		treatRequest(std::string* filename);
	std::string 		findErrorPage(unsigned int errorCode) const;
	std::string			generateResponseHeader(unsigned int requestReturn);
	std::string			generateResponseBody(const std::string& filename);
	//void				findRequestLocation(t_response *response);
	//void				verifyFileAcess(bool isDirectory, Route& routeFound, t_response *response);

	/*-----Set-----*/
	void	setRequest(std::string const &buffer);
	void	setReponse(std::string const &response);
	void	setLastActivity();
	void	reset();

	/*-----Get-----*/
	int					getSocket() const { return this->_sockfd; }
	struct sockaddr_in	getAddr() const { return this->_addr; }
	int					getStatus() const { return this->_status; }
	const Request* 		getRequest() const { return this->_request; }
	const Server*		getServer() const { return this->_server; }
	const std::string&	getResponse() const { return this->_response; }
	time_t				getLastActivity() const { return this->_lastActivity; }


private:
	int					_sockfd;
	struct sockaddr_in	_addr;
	int					_status;
	Request*			_request;
	Server*				_server;
	std::string			_response;
	time_t			_lastActivity;
};

#endif
