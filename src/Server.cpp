/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsohler <lsohler@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 20:30:58 by lsohler           #+#    #+#             */
/*   Updated: 2024/05/21 12:48:04 by lsohler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Response.hpp"
#include "CgiHandler.hpp"

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
#include <algorithm>

#define LISTEN_BACKLOG 42

Server::Server(ServerConfig &new_config) {
	// std::cout << "Creating Server object\n";
	_config = new_config;
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
	// std::cout << "IP: " << ip << "Port: " << port << std::endl;
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

static const std::string cgiExtensions[] = {".cgi", ".pl", ".py", ".sh", ".php", ".rb"};

static bool	isCgi(const std::string& path) {
	size_t dotPosition = path.rfind('.');
	if (dotPosition != std::string::npos) {
		std::string extension = path.substr(dotPosition);

		for (size_t i = 0; i < sizeof(cgiExtensions) / sizeof(cgiExtensions[0]); ++i) {
			if (extension == cgiExtensions[i]) {
				return true;
			}
		}
	}
	return false;
}

const Location*			Server::findCgiLocation(const std::string& path) const {
	const std::map<std::string, Location>&				locations = _config.getLocations();
	size_t dotPosition = path.rfind('.');
	if (dotPosition != std::string::npos) {
		std::string extension = path.substr(dotPosition);

		for (size_t i = 0; i < sizeof(cgiExtensions) / sizeof(cgiExtensions[0]); ++i) {
			if (extension == cgiExtensions[i]) {
				std::map<std::string, Location>::const_iterator	it = locations.find("/*" + extension);
				if (it != locations.end()) {
					it->second.printLocation();
					return &it->second;
				}
			}
		}
	}
	return NULL;
}

const Location*		Server::findLocation(std::string path) const {
	const std::map<std::string, Location>&				locations = _config.getLocations();
	if (isCgi(path)) {
		return	findCgiLocation(path);
	}
	while (true) {
		std::map<std::string, Location>::const_iterator	it = locations.find(path);
		if (path == "/" && it == locations.end()) {
			return NULL;
		}
		if (it != locations.end()) {
			return &it->second;
		} else {
			// Check si / a la fin de find avec un slash et inversement
			if (path[path.size() - 1] == '/') {
				path.erase(path.size() - 1);
			} else {
				path += "/";
			}
			if (locations.find(path) != locations.end()) {
				return new Location(301, path);
			} else {
				path = trimLastLoctation(path);
			}
		}
	}
	return NULL;
}

std::string searchFindReplace(std::string& toSearch, const std::string& toFind, const std::string& toReplace) {
	size_t pos = toSearch.find(toFind);
	if (pos == std::string::npos)
		return toSearch;

	toSearch.replace(pos, toFind.length(), toReplace);
	return toSearch;
}

std::string		Server::findRequestedPath(const Location* location, std::string path) const {
	if (!location) {
		std::cout << "No location\n";
		return "";
	}
	if (location->isCgi()) {
		std::string	cgiRoot = location->getRoot();
		if (!cgiRoot.empty() && cgiRoot.back() == '/' && path[0] == '/') {
			path = path.substr(1);
			std::cout << "cgiRoot is now: " << cgiRoot << "\n";
		}
		std::string realCgiPath = cgiRoot + path;
		std::cout << "Location is cgi: " << realCgiPath << "\n";
		struct stat sbCgi;
		if (stat(realCgiPath.c_str(), &sbCgi) == -1) {
			std::cout << "return NULL for cgi\n";
			return "";
		} else {
			return realCgiPath;
		}
	}
	std::string	realPath = location->getRoot();
	if (location->getLocationName() != path) {
		realPath = searchFindReplace(path, location->getLocationName(), location->getRoot());
	}
	struct stat	sb;
	// std::cout << "Path before check stats: " << realPath << std::endl;
	if (stat(realPath.c_str(), &sb) == -1) {
		return "";
	}
	if (S_ISDIR(sb.st_mode) && !location->getIndex().empty()) {
		realPath += location->getIndex();
	}
	return realPath;
}

static bool	isAutoIndex(const Location* foundLocation, std::string& responseFilePath) {
	struct stat	sb;
	if (stat(responseFilePath.c_str(), &sb) == -1) {
		throw std::runtime_error(std::string("stat: ") + std::strerror(errno));
	}
	if (S_ISDIR(sb.st_mode) && foundLocation->getAutoIndex() && foundLocation->getIndex().empty()) {
		return true;
	}
	return false;
}

static bool	isAllowedMethod(std::vector<std::string> methods, std::string method) {
	return std::find(methods.begin(), methods.end(), method) != methods.end();
}

static bool haveAccess(bool _access, std::string realPath) {
	return _access && access(realPath.c_str(), R_OK) != -1;
}

static bool urlContainRelativePath(std::string realPath) {
	return realPath.find("/.") != std::string::npos || realPath.find("../") != std::string::npos || realPath.find("./") != std::string::npos;
}

unsigned int	Server::generateResponseCode(const Location* location, std::string realPath, const Request& request) const {
	if (!request.isValidRequest()) {
		return 400;
	}
	if (location && location->getReturn().first > 0) {
		return location->getReturn().first;
	}
	if (urlContainRelativePath(request.getURI().path)) {
		return 401;
	}
	if (!location || realPath.empty()) {
		return 404;
	}
	if (location && !location->getCgi().empty()) {
		return	checkCgiError(location, realPath, request);
	}
	if (!haveAccess(location->getAccess(), realPath)) {
		return 403;
	}
	if (!isAllowedMethod(location->getMethods(), request.getMethod())) {
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
			responseFilePath = _config.getErrorDir() + it->second;
				struct stat	sb;
			if (stat(responseFilePath.c_str(), &sb) == -1) {
				responseFilePath =  DEFAULT_ERROR_PAGE;
			}
		} else {
			responseFilePath =  DEFAULT_ERROR_PAGE;
		}
	}
	return responseFilePath;
}

std::string		Server::ResponseRouter(const Request& request) const {
	const Location*		foundLocation = findLocation(request.getURI().path);
	std::string			realPath = findRequestedPath(foundLocation, request.getURI().path);
	// std::cout << "realPath Before1: " << realPath << "\n";
	unsigned int		respCode = generateResponseCode(foundLocation, realPath, request);
	std::string			responseFilePath = generateReponseFilePath(respCode, realPath);
	std::string			response;
	if (foundLocation->isCgi() && respCode == 200) {
		response = CgiHandler::handleCGI(&respCode, foundLocation, responseFilePath, request, &getConfig());
		if (respCode >= 400) {
			responseFilePath = generateReponseFilePath(respCode, realPath);
		} else {
			return response;
		}
	} else if (respCode == 301) {
		response = Response::handleRedir(foundLocation);
		return response;
	} else if (isAutoIndex(foundLocation, responseFilePath)) {
		response = Response::writeAutoIndexPage(responseFilePath);
		return response;
	}
	// std::cout << "Formatter\n";
	response = Response::httpFormatter(responseFilePath, respCode);
	return response;
}
