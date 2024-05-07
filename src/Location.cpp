/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsohler <lsohler@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 11:25:41 by lsohler           #+#    #+#             */
/*   Updated: 2024/05/06 13:39:57 by lsohler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstdlib>

#include "Location.hpp"

Location::Location() :
	_locationName(""),
	_methods(),
	_root(""),
	_cgi(""),
	_upload(""),
	_index(""),
	_return(std::make_pair(0, "")),
	_access(true),
	_listing(false),
	_allocated(false) {
}

Location::Location(const Location& other) :
	_locationName(other._locationName),
	_methods(other._methods),
	_root(other._root),
	_cgi(other._cgi),
	_upload(other._upload),
	_index(other._index),
	_return(other._return),
	_access(other._access),
	_listing(other._listing),
	_allocated(other._allocated) {
}

Location::~Location() {}

Location	&Location::operator=(const Location& other) {
	if (this != &other) {
		_locationName = other._locationName;
		_methods = other._methods;
		_root = other._root;
		_cgi = other._cgi;
		_upload = other._upload;
		_index = other._index;
		_return = other._return;
		_access = other._access;
		_listing = other._listing;
		_allocated = other._allocated;
	}
	return *this;
}

typedef void (Location::*SetterFunction)(const std::string&);

static void tokenSetter(std::vector<std::string>& tokens, Location& config, SetterFunction setter) {
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

void	locationHandleMethods(Location &location, std::vector<std::string> &tokens) {
	tokenSetter(tokens, location, &Location::setMethods);
}

void	locationHandleRoot(Location &location, std::vector<std::string> &tokens) {
	tokenSetter(tokens, location, &Location::setRoot);

}

void	locationHandleCgi(Location &location, std::vector<std::string> &tokens) {
	tokenSetter(tokens, location, &Location::setCgi);
}

void	locationHandleUpload(Location &location, std::vector<std::string> &tokens) {
	tokenSetter(tokens, location, &Location::setUpload);
}

void	locationHandleIndex(Location &location, std::vector<std::string> &tokens) {
	tokenSetter(tokens, location, &Location::setIndex);
}

void	locationHandleReturn(Location &location, std::vector<std::string> &tokens) {
	std::pair<unsigned int, std::string> returnPair;
	returnPair.first = 0;
	returnPair.second = "";
	tokens.erase(tokens.begin());
	while (!tokens.empty()) {
		if (*tokens.begin() == ";") {
			tokens.erase(tokens.begin());
			break;
		}
		returnPair.first = atoi((*tokens.begin()).c_str());
		if (*tokens.begin() == ";") {
			tokens.erase(tokens.begin());
			break;
		}
		tokens.erase(tokens.begin());
		returnPair.second = (*tokens.begin());
		tokens.erase(tokens.begin());
	}
	location.setReturn(returnPair);
}

void	locationHandleAccess(Location &location, std::vector<std::string> &tokens) {
	tokenSetter(tokens, location, &Location::setAccess);
}

void	locationHandleListing(Location &location, std::vector<std::string> &tokens) {
	tokenSetter(tokens, location, &Location::setListing);
}

typedef void (*locationHandler)(Location &, std::vector<std::string>&);

std::map<std::string, locationHandler> locationMap() {
	std::map<std::string, locationHandler> myMap;

	myMap["methods"] = &locationHandleMethods;
	myMap["root"] = &locationHandleRoot;
	myMap["cgi"] = &locationHandleCgi;
	myMap["upload"] = &locationHandleUpload;
	myMap["index"] = &locationHandleIndex;
	myMap["return"] = &locationHandleReturn;
	myMap["access"] = &locationHandleAccess;
	myMap["listing"] = &locationHandleListing;

	return myMap;
}

void	tokenLocationNotRecognized(std::vector<std::string> &tokens) {
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

Location::Location(unsigned int redirCode, std::string redirPath) :
	_locationName(""),
	_methods(),
	_root(""),
	_cgi(""),
	_upload(""),
	_index(""),
	_return(std::make_pair(redirCode, redirPath)),
	_access(true),
	_listing(false),
	_allocated(true) {
}

Location::Location(std::vector<std::string> &tokens):
	_locationName(""),
	_methods(),
	_root(""),
	_cgi(""),
	_upload(""),
	_index(""),
	_return(std::make_pair(0, "")),
	_access(true),
	_listing(false),
	_allocated(false)
{
	std::map<std::string, locationHandler> map = locationMap();

	tokens.erase(tokens.begin());
	if (*tokens.begin() != "{") {
		_locationName = *tokens.begin();
		tokens.erase(tokens.begin());
	}
	else {
		std::cerr << "Location parsing error." << std::endl;
		exit(1);
	}
	tokens.erase(tokens.begin());
	while (!tokens.empty()) {
		if (map.find(*tokens.begin()) != map.end()) {
			locationHandler function = map[*tokens.begin()];
			function(*this, tokens);
		}
		else if (*tokens.begin() == "}") {
			tokens.erase(tokens.begin());
			break ;
		}
		else {
			tokenLocationNotRecognized(tokens);
		}
	}
}

void	Location::printLocation() const {
	std::cout << "  Location: " << _locationName << std::endl;
	std::cout << "	Methods:";
	for (size_t i = 0; i < _methods.size(); ++i) {
			std::cout << " " << _methods[i];
	}
	std::cout << std::endl;
	std::cout << "	Root: " << _root << std::endl;
	std::cout << "	CGI: " << _cgi << std::endl;
	std::cout << "	Upload: " << _upload << std::endl;
	std::cout << "	Index: " << _index << std::endl;
	std::cout << "	Access: " << (_access ? "true" : "false") << std::endl;
	std::cout << "	Listing: " << (_listing ? "true" : "false") << std::endl;
	std::cout << "	Return: " << _return.first << " " << _return.second << std::endl;
}
