#include "Server.hpp"

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netdb.h>
#include <unistd.h>

#include <string>
#include <cstring>
#include <sstream>
#include <iostream>
#include <ostream>
#include <cerrno>

#define LISTEN_BACKLOG 42

Server::Server(ServerConfig &new_config) {
	std::cout << "Creating Server object\n";
	_config = new_config;
	// new_config.printServerConfig();
	int err;
	struct addrinfo *resp;
	struct addrinfo *rp;
	struct addrinfo hints;
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	std::string	ip = this->getConfig().getIP();
	std::string	port = this->getConfig().getPort();
	std::cout << "IP: " << ip << "Port: " << port << std::endl;
	if (ip.empty()) {
		ip = "0.0.0.0";
	}
	err = getaddrinfo(ip.c_str(), port.c_str(), &hints, &resp);
	if (err != 0) {
		throw std::runtime_error(std::string("getaddrinfo: ") + gai_strerror(err));
	}

	for (rp = resp; rp != NULL; rp = rp->ai_next) {
		this->_sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		int enable = 1;
		if (setsockopt(this->_sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) == -1) {
			throw std::runtime_error(std::string("setsockopt: ") + std::strerror(errno));
		}
		if (this->_sockfd == -1) {
			continue;
		}
		if (bind(this->_sockfd, rp->ai_addr, rp->ai_addrlen) != -1) {
			break;
		}
		if (close(this->_sockfd) == -1) {
			throw std::runtime_error(std::string("close: ") + std::strerror(errno));
		}
	}
	freeaddrinfo(resp);
	if (rp == NULL) {
		throw std::runtime_error(std::string("Cannot create socket: ") + std::strerror(errno));
	}
	this->_isRunning = false;
}

Server::~Server() {
	if (close(this->_sockfd) == -1) {
		throw std::runtime_error(std::string("close: ") + std::strerror(errno));
	}
	this->_isRunning = false;
	std::cout << "close socket: " << this->_sockfd << std::endl;
}

void Server::run() {
	if (!this->_isRunning) {
		if (listen(this->_sockfd, LISTEN_BACKLOG) == -1) {
			throw std::runtime_error(std::string("listen: ") + std::strerror(errno));
		}
		this->_isRunning = true;
		std::cout << "Server running on socket: " << this->_sockfd
				  << ", listening on: " << this->getConfig().getIP() << ":" << this->getConfig().getPort() << std::endl;
	}
}

std::ostream &operator<<(std::ostream &os, Server const &ref) {
	os << "ServerSocket { socket: " << ref.getSocket()
	   << ", IP: " << ref.getConfig().getIP()
	   << ", port: " << ref.getConfig().getPort()
	   << ", running: " << std::boolalpha << ref.isRunning() << std::noboolalpha
	   << " }";
	return os;
}

static std::string trimLastLoctation(std::string chaine) {
    if (chaine.empty())
        return chaine;

    size_t pos = chaine.rfind('/');
    if (pos == chaine.size() - 1 && chaine.size() != 1)
        chaine = chaine.substr(0, pos);
	pos = chaine.rfind('/');
    if (pos == std::string::npos)
        return "/";

    if (pos == 0 && chaine.size() == 1)
        return "/";

    return chaine.substr(0, pos) + "/";
}

const Route*		Server::findLocation(std::string path) const {
	const Route*		foundRoute = NULL;
	const std::map<std::string, Route>&				routes = _config.getRoutes();
	// std::string	searchedLocation = _searchedLocation;
	while (true) {
		std::map<std::string, Route>::const_iterator	it = routes.find(path);
		if (it != routes.end()) {
			if (it->second._return.first != 0) {
				if (it->second._return.first == 404) {
					// return 404;
					return NULL;
				}
				// std::cout << "\n\nRoute rediction found:\n";
				// _config.printRoute(it->second);
				// std::cout << "foundRoute._return.second: " << foundRoute._return.second << std::endl;
				foundRoute = &(routes.find(it->second._return.second))->second;
				return foundRoute;
				// redir = it->second._return.first;
				// _config.printRoute(foundRoute);
				// _realPath = searchFindReplace(_searchedLocation, foundRoute.location, foundRoute.root);
				// return 301;
			} else {
				foundRoute = &it->second;
				break;
			}
		} else {
			std::cout << "searchLocation 1: " << path << std::endl;
			path = trimLastLoctation(path);
			std::cout << "searchLocation 2: " << path << std::endl;
			it = routes.find(path);
		}
		if (path == "/" && routes.find(path) == routes.end()) {
			// return 404;
			return NULL;
		}
	}
	return foundRoute;
}

std::string searchFindReplace(std::string& toSearch, const std::string& toFind, const std::string& toReplace) {
    size_t pos = toSearch.find(toFind);
    if (pos == std::string::npos)
        return toSearch;

    toSearch.replace(pos, toFind.length(), toReplace);
    return toSearch;
}

std::string		Server::findRequestedPath(const Route* route, std::string path) const {
	if (!route) {
		return "";
	}
	std::string	realPath = route->root;
	// if (route->_return.first == 301) {
	// 	realPath = searchFindReplace(path, route->location, route->_return.second);
	// }
	if (route->location != path) {
		realPath = searchFindReplace(path, route->location, route->root);
		std::cout << "IF 1: " << realPath << std::endl;
	}
	struct stat	sb;
	if (stat(realPath.c_str(), &sb) == -1) {
		return "";
		// throw std::runtime_error(std::string("stat: ") + std::strerror(errno));
	}
	//TODO: checker nginx
	if (S_ISDIR(sb.st_mode) && !route->listing) {
		realPath += route->index;
	}
	return realPath;
}

static bool	isAllowedMethod(std::vector<std::string> methods, std::string method) {
	return std::find(methods.begin(), methods.end(), method) != methods.end();
}

static bool haveAccess(bool _access, std::string realPath) {
	return _access && access(realPath.c_str(), R_OK) != -1;
}

unsigned int	Server::generateResponseCode(const Route* route, std::string realPath, const Request& request) const {
	if (!route || realPath.empty()) {
		return 404;
	}
	if (!haveAccess(route->access, realPath)) {
		return 403;
	}
	if (!isAllowedMethod(route->methods, request.getMethod())) {
		return 405;
	}
	return 200;
}


std::string		Server::generateReponseFilePath(unsigned int responseCode, std::string realPath) const {
	std::string responseFilePath = realPath;

	if (responseCode >= 400) {
		std::map<int, std::string>					error_pages = _config.getErrorPage();
		std::map<int, std::string>::const_iterator	it = error_pages.find(responseCode);

		if (it != error_pages.end()) {
			responseFilePath = _config.getRoot() + "default_error/" + it->second;
		} else {
			responseFilePath =  DEFAULT_ERROR_PAGE;
		}
	}
	return responseFilePath;
}