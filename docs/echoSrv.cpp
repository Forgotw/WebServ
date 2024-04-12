#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#include <iostream>
#include <cerrno>
#include <cstring>

#define LISTEN_BACKLOG 42
#define ERROR -1

# define RESET "\e[0m"
# define RED "\e[31m"
# define GREEN "\e[32m"
# define CYAN "\e[36m"

static int makeSocket(std::string const &ipAddr, std::string const &port);
static bool launchServer(int fd);
static void process(int masterSock);
static bool handleNewConnection(int masterSock, fd_set *readfds, int *peerSockets);
static bool handlePeerRequest(fd_set *readfds, int *peerSockets);

int main () {
	int fd_master = makeSocket("127.0.0.1", "8080");
	if (fd_master == -1)
		return 1;
	int flags = fcntl(fd_master, F_GETFL, 0);
	fcntl(fd_master, F_SETFL, flags | O_NONBLOCK);
	if (!launchServer(fd_master)) {
		close(fd_master);
		return 2;
	}
	process(fd_master);
	return 0;
}

static int makeSocket(std::string const &ipAddr, std::string const &port) {
	int newSocket;
	int s;
	struct addrinfo *resp;
	struct addrinfo *rp;
	struct addrinfo hints;
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	s = getaddrinfo(ipAddr.c_str(), port.c_str(), &hints, &resp);
	if (s != 0) {
		std::cerr << "[!] getaddrinfo(): " << gai_strerror(s) << std::endl;
		return ERROR;
	}

	for (rp = resp; rp != NULL; rp = rp->ai_next) {
		newSocket = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (newSocket == -1)
			continue;
		if (bind(newSocket, rp->ai_addr, rp->ai_addrlen) != -1)
			break;
		close(newSocket);
	}
	freeaddrinfo(resp);
	if (rp == NULL) {
		std::cerr << "[!] Unable to make socket" << std::endl;
		return ERROR;
	}
	return newSocket;
}

static bool launchServer(int fd) {
	if (listen(fd, LISTEN_BACKLOG) == -1) {
		std::cerr << "[!] listen(): " << std::strerror(errno) << std::endl;
		return false;
	}
	std::cout << CYAN << "Server running" << RESET << std::endl;
	return true;
}

static void process(int masterSock) {
	int activity;
	int peerSockets[FD_SETSIZE];
	for (int i = 0; i < FD_SETSIZE; i++) {
		peerSockets[i] = 0;
	}

	for(;;) {
		fd_set readfds;

		FD_ZERO(&readfds);
		FD_SET(masterSock, &readfds);
		for (int i = 0; i < FD_SETSIZE; i++) {
			if (peerSockets[i] > 0) {
				FD_SET(peerSockets[i], &readfds);
			}
		}

		activity = select(FD_SETSIZE, &readfds, NULL, NULL, 0);
		if (activity < 0) {
			std::cerr << "[!] select() : " << std::strerror(errno);
			return;
		}
		if (activity > 0) {
			if (!handleNewConnection(masterSock, &readfds, peerSockets))
				break;
			if (!handlePeerRequest(&readfds, peerSockets))
				break;
		}
	}
	for (int i = 0; i < FD_SETSIZE; i++) {
		if (peerSockets[i] > 0) {
			close(peerSockets[i]);
			peerSockets[i] = 0;
		}
	}
	if (masterSock != -1)
		close(masterSock);
}

static bool handleNewConnection(int masterSock, fd_set *readfds, int *peerSockets) {
	if (FD_ISSET(masterSock, readfds)) {
		int newSocket;

		struct sockaddr_in peerSocketAddr;
		int peerSocketAddrLen = sizeof(peerSocketAddr);
		if ((newSocket = accept(masterSock, (struct sockaddr *) &peerSocketAddr, (socklen_t *) &peerSocketAddrLen)) < 0) {
			std::cerr << "[!] accept() : " << std::strerror(errno) << std::endl;
			return false;
		}
		int flags = fcntl(newSocket, F_GETFL, 0);
		fcntl(newSocket, F_SETFL, flags | O_NONBLOCK);
		std::cout << GREEN << "Connect:\t";
		std::cout << "Socket fd : " << newSocket;
		std::cout << " | Peer ip : "
			<< (unsigned int)(peerSocketAddr.sin_addr.s_addr & 0xFF) << "."
			<< (unsigned int)((peerSocketAddr.sin_addr.s_addr & 0xFF00) >> 8) << "."
			<< (unsigned int)((peerSocketAddr.sin_addr.s_addr & 0xFF0000) >> 16) << "."
			<< (unsigned int)((peerSocketAddr.sin_addr.s_addr & 0xFF000000) >> 24);
		std::cout << " | Peer port : " << ntohs(peerSocketAddr.sin_port) << RESET << std::endl;
		for (int i = 0; i < FD_SETSIZE; i++) {
			if (peerSockets[i] == 0) {
				peerSockets[i] = newSocket;
				break;
			}
		}
	}
	return true;
}

static bool handlePeerRequest(fd_set *readfds, int *peerSockets) {
	ssize_t bytes_read;
	char buffer[1024];

	for (int i = 0; i < FD_SETSIZE; i++) {
		if (FD_ISSET(peerSockets[i], readfds)) {
			bytes_read = read(peerSockets[i], buffer, 1024);
			if (bytes_read < 0) {
				if (errno == EWOULDBLOCK || errno == EAGAIN)
					continue;
				std::cerr << "[!] read : " << std::strerror(errno) << std::endl;
				return false;
			}
			else if (bytes_read == 0) {
				struct sockaddr_in peerAddr;
				int peerAddrLen = sizeof(peerAddr);
				if (getsockname(peerSockets[i], (struct sockaddr *) &peerAddr, (socklen_t *) &peerAddrLen) == -1) {
					std::cerr << "[!] getsockname() : " << std::strerror(errno) << std::endl;
					return false;
				}
				std::cout << RED << "Disconnect:\t";
				std::cout << "Socket fd : " << peerSockets[i];
				std::cout << " | Peer ip : "
					<< (unsigned int)(peerAddr.sin_addr.s_addr & 0xFF) << "."
					<< (unsigned int)((peerAddr.sin_addr.s_addr & 0xFF00) >> 8) << "."
					<< (unsigned int)((peerAddr.sin_addr.s_addr & 0xFF0000) >> 16) << "."
					<< (unsigned int)((peerAddr.sin_addr.s_addr & 0xFF000000) >> 24);
				std::cout << " | Peer port : " << ntohs(peerAddr.sin_port) << RESET << std::endl;
				close(peerSockets[i]);
				peerSockets[i] = 0;
			} else {
				buffer[bytes_read] = '\0';
				if (std::strncmp(buffer, "exit --force\n", bytes_read) == 0) {
					return false;
				}
				int bytes_written = write(peerSockets[i], buffer, std::strlen(buffer));
				if (bytes_written < 0)
				{
					if (errno == EWOULDBLOCK || errno == EAGAIN) {
						continue;
					} else {
						std::cerr << "[!] write : " << std::strerror(errno) << std::endl;
					}
				}
			}
		}
	}
	return true;
}
