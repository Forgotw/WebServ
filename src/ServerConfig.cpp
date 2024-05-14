/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: efailla <efailla@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/12 16:26:12 by lsohler           #+#    #+#             */
/*   Updated: 2024/05/14 17:25:18 by efailla          ###   ########.fr       */
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

void	handleErrorDir(ServerConfig &config, std::vector<std::string> &tokens) {
	tokenSetter(tokens, config, &ServerConfig::setErrorDir);
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
	myMap["error_dir"] = &handleErrorDir;
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
	if (!isValidServerConfig())
		std::cout << "config NON valide" << std::endl;
	else
		std::cout << "config valide" << std::endl;
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

bool ServerConfig::isValidIPAddress()
{
    int num1, num2, num3, num4;
    char end;

    if (sscanf(_ip.c_str(), "%d.%d.%d.%d%c", &num1, &num2, &num3, &num4, &end) == 4) {
        return (num1 >= 0 && num1 <= 255) &&
               (num2 >= 0 && num2 <= 255) &&
               (num3 >= 0 && num3 <= 255) &&
               (num4 >= 0 && num4 <= 255);
    }
    return false;
}

bool ServerConfig::isValidPort()
{
	std::string portS = _port;
     if (portS.empty()) {
        return false;
    }
    int port = 0;
    for (size_t i = 0; i < portS.size(); i++) {
        if (!isdigit(_port[i])) {
            return false;
        }
        port = 10 * port + (portS[i] - '0');
        if (port > 65535) {
            return false;
        }
    }
    return true;
}

bool ServerConfig::isValidServerName() {
    for (size_t i = 0; i < _server_name.length(); ++i) {
        char currentChar = _server_name[i];
        if (!(isalnum(currentChar) || currentChar == '-' || currentChar == '.')) {
            return false;
        }
    }
    return true;
}

bool hasDuplicates(const std::vector<std::string>& vec) {
    for (size_t i = 0; i < vec.size(); ++i) {
        for (size_t j = i + 1; j < vec.size(); ++j) {
            if (vec[i] == vec[j]) {
                return true;
            }
        }
    }
    return false;
}

bool isValidHttpMethods(const std::vector<std::string>& methods) {
    // if (methods.empty()) {
    //     return false;
    // }
    if (hasDuplicates(methods)) {
        return false;
    }
    for (size_t i = 0; i < methods.size(); ++i) {
        if (methods[i] != "POST" && methods[i] != "GET" && methods[i] != "DELETE") {
            return false;
        }
    }
    return true;
}

bool isPathAccessible(const std::string& path) {
    if (access(path.c_str(), F_OK) != -1) {
        return true;
    } else {
        return false;
    }
}

bool isFileAccessible(const std::string& filePath) {
    if (access(filePath.c_str(), R_OK) != -1) {
        return true;
    } else {
        return false;
    }
}

bool	isValidReturn(std::string returnLoc, std::map<std::string, Location> &locations)
{
	std::map<std::string, Location>::iterator it;
	for (it = locations.begin(); it != locations.end(); ++it)
	{
		if (returnLoc == it->first)
			break;
	}
	if (it == locations.end())
		return false;
	else
		return true;
}

bool ServerConfig::isValidLocation() {
    if (_locations.empty())
        return false;
    
    std::map<std::string, Location>::iterator it;
    for (it = _locations.begin(); it != _locations.end(); ++it) {
        if (!isValidHttpMethods(it->second.getMethods()))
		{
			std::cout << "methods non valide" << std::endl;
            return false;
		}
		if (!it->second.getReturn().second.empty() && !isValidReturn(it->second.getReturn().second, _locations))
        {
			std::cout << "return non valide" << std::endl;
            return false;
		}
        if (it->second.getReturn().second.empty() && !isPathAccessible(it->second.getRoot()))
        {
			std::cout << "root non valide" << std::endl;
            return false;
		}
        if (!it->second.getUpload().empty() && !isPathAccessible(it->second.getUpload()))
        {
			std::cout << "upload non valide" << std::endl;
            return false;
		}
        if (!it->second.getIndex().empty() && !isFileAccessible(it->second.getRoot() + it->second.getIndex()))
        {
			std::cout << "index non valide" << std::endl;
            return false;
		}
    }
    return true;
}

bool	ServerConfig::isValidServerConfig() {
	// if (!isValidIPAddress())
	// 	return false;
	if (!isValidPort())
	{
		std::cout << "Port non valide" << std::endl;
		return false;
	}
	if (!_error_dir.empty() && !isPathAccessible(_error_dir))
	{
		std::cout << "error dir non valide" << std::endl;
		std::cout << _error_dir << std::endl;
		return false;
	}
	if (_client_max_body_size < 0)
	{
		std::cout << "client max body size non valide" << std::endl;
		return false;
	}
	// if (!isValidServerName())
	// {
	// 	std::cout << "server name non valide" << std::endl;
	// 	return false;
	// }
	if (!isValidLocation())
	{
		std::cout << "location non valide" << std::endl;
		return false;
	}
	return true;
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
