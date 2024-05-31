/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsohler <lsohler@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/12 16:26:12 by lsohler           #+#    #+#             */
/*   Updated: 2024/05/31 18:16:02 by lsohler          ###   ########.fr       */
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

bool isValidIP(const std::string& ip) {
    std::istringstream iss(ip);
    std::string token;
    int count = 0;
    while (std::getline(iss, token, '.')) {
        if (++count > 4) return false; // Plus de 4 parties signifie que ce n'est pas une adresse IP valide
        for (size_t i = 0; i < token.size(); ++i) {
            if (!isdigit(token[i])) return false; // Si un caractère n'est pas un chiffre, ce n'est pas une adresse IP valide
        }
        int num = std::atoi(token.c_str());
        if (num < 0 || num > 255) return false; // Chaque partie doit être entre 0 et 255
    }
    return count == 4; // Il doit y avoir exactement 4 parties
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

void	handleHost(ServerConfig &config, std::vector<std::string> &tokens) {
	tokens.erase(tokens.begin());
	while (!tokens.empty()) {
		if (*tokens.begin() == ";") {
			tokens.erase(tokens.begin());
			break;
		}
        if (isValidIP(*tokens.begin())) {
    		config.setIP(*tokens.begin());
        } else if (tokens.begin()->find('.') == std::string::npos) { // FIXEME: Faut-il exclure server name si il y a un . dedans ? genre google.com
            config.setServerName(*tokens.begin());
        }
		tokens.erase(tokens.begin());
	}
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
	tokenSetter(tokens, config, &ServerConfig::setErrorPage);
}

void	handleRoot(ServerConfig &config, std::vector<std::string> &tokens) {
	tokenSetter(tokens, config, &ServerConfig::setRoot);
}

void	handleIndex(ServerConfig &config, std::vector<std::string> &tokens) {
	tokenSetter(tokens, config, &ServerConfig::setIndex);
}

void	handleUpload(ServerConfig &config, std::vector<std::string> &tokens) {
	tokenSetter(tokens, config, &ServerConfig::setUpload);
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
    std::cout << "handleLocation: " << *tokens.begin() << std::endl;
    tokens.erase(tokens.begin());
    std::cout << "handleLocation 2: " << *tokens.begin() << std::endl;
    if (tokens.empty()) {
        std::cout << "Locations return;\n";
        return ;
    }
    if (*tokens.begin() == "~") {
        std::cout << "handleLocation 3: " << *tokens.begin() << std::endl;
        tokens.erase(tokens.begin());
        std::cout << "Found cgi location: " << *tokens.begin() << std::endl;
        Location	newLocation(tokens);
        config.setCgiLocations(newLocation);
    } else {
        Location	newLocation(tokens);
        config.setLocations(newLocation);
    }
}

typedef void (*caseHandler)(ServerConfig&, std::vector<std::string>&);

std::map<std::string, caseHandler> caseMap() {
	std::map<std::string, caseHandler> myMap;

	myMap["host"] = &handleHost;
	myMap["listen"] = &handleListen;
	myMap["server_name"] = &handleServerName;
	myMap["access_log"] = &handleAccessLog;
	myMap["error_log"] = &handleErrorLog;
	myMap["error_page"] = &handleErrorPage;
	myMap["error_dir"] = &handleErrorDir;
	myMap["root"] = &handleRoot;
	myMap["index"] = &handleIndex;
	myMap["upload"] = &handleUpload;
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
    _host(),
	_client_max_body_size(DEF_MAX_BODY_SIZE),
	_server_name(),
	_access_log(""),
	_error_log(""),
	_root(""),
	_index(""),
	_upload(""),
	_locations(),
	_cgiLocations()
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
    _host = getServerName().empty() ? getIP() : getServerName();
}

ServerConfig::ServerConfig(void) :
	_ip(),
	_port(),
	_host(),
	_client_max_body_size(0),
	_server_name(),
	_access_log(""),
	_error_log(""),
	_error_page(),
	_root(""),
	_index(""),
	_upload(""),
	_locations(),
	_cgiLocations() {
}

ServerConfig::ServerConfig(ServerConfig const &other) :
	_ip(other._ip),
	_port(other._port),
	_host(other._host),
	_client_max_body_size(other._client_max_body_size),
	_server_name(other._server_name),
	_access_log(other._access_log),
	_error_log(other._error_log),
	_error_page(other._error_page),
	_root(other._root),
	_index(other._index),
	_upload(other._upload),
	_locations(other._locations),
	_cgiLocations(other._cgiLocations) {
}

ServerConfig::~ServerConfig(void) {
}

ServerConfig	&ServerConfig::operator=(ServerConfig const &other) {
	if (this != &other) {
		_ip = other._ip;
		_port = other._port;
		_host = other._host;
		_client_max_body_size = other._client_max_body_size;
		_server_name = other._server_name;
		_access_log = other._access_log;
		_error_log = other._error_log;
		_error_page = other._error_page;
		_root = other._root;
		_index = other._index;
		_upload = other._upload;
		_locations = other._locations;
		_cgiLocations = other._cgiLocations;
	}
	return *this;
}

bool	ServerConfig::isValidServerConfig() {
	return true;
}

/*--------------TESTING FUNCTIONS--------------*/


void	ServerConfig::printServerConfig(void) const {
	std::cout << "IP: " << _ip << std::endl;
	std::cout << "Port: " << _port << std::endl;
	std::cout << "Host: " << _host << std::endl;
	std::cout << "Client Max Body Size: " << _client_max_body_size << std::endl;
	std::cout << "Server Name: " << _server_name << std::endl;
	std::cout << "Access Log: " << _access_log << std::endl;
	std::cout << "Error Log: " << _error_log << std::endl;
	std::cout << "Root: " << _root << std::endl;
	std::cout << "Index: " << _index << std::endl;
	std::cout << "Upload: " << _upload << std::endl;
	std::cout << std::endl;
	std::cout << "---Locations---" << std::endl;
	for (std::map<std::string, Location>::const_iterator it = _locations.begin(); it != _locations.end(); ++it) {
		it->second.printLocation();
	}
	std::cout << "---cgiLocations---" << std::endl;
	for (std::map<std::string, Location>::const_iterator it = _cgiLocations.begin(); it != _cgiLocations.end(); ++it) {
		it->second.printLocation();
	}
}
