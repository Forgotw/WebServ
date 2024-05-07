/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsohler <lsohler@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/12 16:26:12 by lsohler           #+#    #+#             */
/*   Updated: 2024/05/07 14:05:59 by lsohler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include "ServerConfig.hpp"


unsigned int	maxBodySizeConverter(const std::string& maxSizeString) {
	unsigned int maxSize = std::atoi(maxSizeString.c_str());

	if (maxSize == 0)
		return DEF_MAX_BODY_SIZE;
	if (maxSizeString.size() > 2) {
		std::string suffix = maxSizeString.substr(maxSizeString.size() - 2);
		if (suffix == "ko" || suffix == "Ko") {
			maxSize *= 1024;
		} else if (suffix == "mo" || suffix == "Mo") {
			maxSize *= 1024 * 1024;
		} else if (suffix == "go" || suffix == "Go") {
			maxSize *= 1024 * 1024 * 1024;
		}
	}
	return maxSize;
}

void	printTokenUntilSemicolon(std::vector<std::string> &tokens) {
	while (!tokens.empty()) {
		std::cout << " " << *tokens.begin();
		tokens.erase(tokens.begin());
		if (*tokens.begin() == ";") {
			tokens.erase(tokens.begin());
			break ;
		}
	}
	std::cout << std::endl;
}

typedef void (ServerConfig::*SetterFunction)(const std::string&);

static void tokenSetter(std::vector<std::string>& tokens, ServerConfig &config, SetterFunction setter) {
	tokens.erase(tokens.begin());
	while (!tokens.empty()) {
		if (*tokens.begin() == ";") {
			tokens.erase(tokens.begin());
			break;
		}
		(config.*setter)(*tokens.begin());
		tokens.erase(tokens.begin());
	}
}

void	handleListen(ServerConfig &config, std::vector<std::string> &tokens) {
	tokenSetter(tokens, config, &ServerConfig::setPort);
}

void	handleIP(ServerConfig &config, std::vector<std::string> &tokens) {
	tokenSetter(tokens, config, &ServerConfig::setIP);
}

void	handleClientMaxBodySize(ServerConfig &config, std::vector<std::string> &tokens) {
	tokenSetter(tokens, config, &ServerConfig::setClientMaxBodySize);
}

void	handleServerName(ServerConfig &config, std::vector<std::string> &tokens) {
	tokenSetter(tokens, config, &ServerConfig::setServerName);
}

void	handleAccessLog(ServerConfig &config, std::vector<std::string> &tokens) {
	tokenSetter(tokens, config, &ServerConfig::setAccessLog);
}

void	handleErrorLog(ServerConfig &config, std::vector<std::string> &tokens) {
	tokenSetter(tokens, config, &ServerConfig::setErrorLog);
}

void	handleErrorPage(ServerConfig &config, std::vector<std::string> &tokens) {
	tokenSetter(tokens, config, &ServerConfig::setErrorPage);
}

void	handleRoot(ServerConfig &config, std::vector<std::string> &tokens) {
	tokenSetter(tokens, config, &ServerConfig::setRoot);
}

void	handleIndex(ServerConfig &config, std::vector<std::string> &tokens) {
	tokenSetter(tokens, config, &ServerConfig::setIndex);
}

void	tokenNotRecognized(std::vector<std::string> &tokens) {
	std::cerr << "TOKEN NOT RECOGNIZED: ";
	while (!tokens.empty()) {
		std::cerr << " " << *tokens.begin();
		tokens.erase(tokens.begin());
		if (*tokens.begin() == ";") {
			tokens.erase(tokens.begin());
			break ;
		}
	}
	std::cerr << std::endl;
}

void	handleLocation(ServerConfig &config, std::vector<std::string> &tokens) {
	Location	newLocation(tokens);
	config.setLocations(newLocation);
}

typedef void (*caseHandler)(ServerConfig&, std::vector<std::string>&);

std::map<std::string, caseHandler> caseMap() {
	std::map<std::string, caseHandler> myMap;

	myMap["ip"] = &handleIP;
	myMap["listen"] = &handleListen;
	myMap["server_name"] = &handleServerName;
	myMap["access_log"] = &handleAccessLog;
	myMap["error_log"] = &handleErrorLog;
	myMap["error_page"] = &handleErrorPage;
	myMap["root"] = &handleRoot;
	myMap["index"] = &handleIndex;
	myMap["client_max_body_size"] = &handleClientMaxBodySize;
	myMap["location"] = &handleLocation;

	return myMap;
}

/*-----------CONSTRUCTOR DESTRUCTOR------------*/
bool keyExists(const std::string& key, std::map<std::string, caseHandler> map) {
	return map.find(key) != map.end();
}

ServerConfig::ServerConfig(std::vector<std::string> tokens) :
	_ip(),
	_port(),
	_client_max_body_size(DEF_MAX_BODY_SIZE),
	_server_name(),
	_access_log(""),
	_error_log(""),
	_root(""),
	_index(""),
	_locations()
{
	std::map<std::string, caseHandler> map = caseMap();
	// for (std::vector<std::string>::iterator it = tokens.begin(); it != tokens.end(); ++it) {
	while (!tokens.empty()) {
		if (keyExists(*tokens.begin(), map)) {
			caseHandler function = map[*tokens.begin()];
			function(*this, tokens);
			// std::cout << "After caseHandler: " << *tokens.begin() << std::endl;
		}
		else {
			tokenNotRecognized(tokens);
		}
	}
}

ServerConfig::ServerConfig(void) :
	_ip(),
	_port(),
	_client_max_body_size(0),
	_server_name(),
	_access_log(""),
	_error_log(""),
	_error_page(),
	_root(""),
	_index(""),
	_locations() {
}

ServerConfig::ServerConfig(ServerConfig const &other) :
	_ip(other._ip),
	_port(other._port),
	_client_max_body_size(other._client_max_body_size),
	_server_name(other._server_name),
	_access_log(other._access_log),
	_error_log(other._error_log),
	_error_page(other._error_page),
	_root(other._root),
	_index(other._index),
	_locations(other._locations) {
}

ServerConfig::~ServerConfig(void) {
}

ServerConfig	&ServerConfig::operator=(ServerConfig const &other) {
	if (this != &other) {
		_ip = other._ip;
		_port = other._port;
		_client_max_body_size = other._client_max_body_size;
		_server_name = other._server_name;
		_access_log = other._access_log;
		_error_log = other._error_log;
		_error_page = other._error_page;
		_root = other._root;
		_index = other._index;
		_locations = other._locations;
	}
	return *this;
}

/*--------------TESTING FUNCTIONS--------------*/


void	ServerConfig::printServerConfig(void) const {
	std::cout << "IP:" << _ip << std::endl;
	std::cout << "Port:" << _port << std::endl;
	std::cout << "Client Max Body Size: " << _client_max_body_size << std::endl;
	std::cout << "Server Name: " << _server_name << std::endl;
	std::cout << "Access Log: " << _access_log << std::endl;
	std::cout << "Error Log: " << _error_log << std::endl;
	std::cout << "Root: " << _root << std::endl;
	std::cout << "Index:" << _index << std::endl;
	std::cout << std::endl;
	std::cout << "Locations:" << std::endl;
	for (std::map<std::string, Location>::const_iterator it = _locations.begin(); it != _locations.end(); ++it) {
		it->second.printLocation();
	}
}
