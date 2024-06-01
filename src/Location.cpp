/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsohler <lsohler@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 11:25:41 by lsohler           #+#    #+#             */
/*   Updated: 2024/06/01 16:45:32 by lsohler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

Location::Location() :
	_locationName(""),
	_methods(),
	_root(""),
	_cgi(""),
    _fast_cgi_pass(""),
	_upload(""),
	_index(""),
    _client_max_body_size(0),
	_return(std::make_pair(0, "")),
	_access(true),
	_autoindex(false),
	_allocated(false) {
}

Location::Location(const Location& other) :
	_locationName(other._locationName),
	_methods(other._methods),
	_root(other._root),
	_cgi(other._cgi),
    _fast_cgi_pass(other._fast_cgi_pass);
	_upload(other._upload),
	_index(other._index),
	_client_max_body_size(other._client_max_body_size),
	_return(other._return),
	_access(other._access),
	_autoindex(other._autoindex),
	_allocated(other._allocated) {
}

Location::~Location() {}

Location	&Location::operator=(const Location& other) {
	if (this != &other) {
		_locationName = other._locationName;
		_methods = other._methods;
		_root = other._root;
		_cgi = other._cgi;
        _fast_cgi_pass = other._fast_cgi_pass;
		_upload = other._upload;
		_index = other._index;
        _client_max_body_size = other._client_max_body_size;
		_return = other._return;
		_access = other._access;
		_autoindex = other._autoindex;
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

void	locationHandleFastCgiPass(Location &location, std::vector<std::string> &tokens) {
	tokenSetter(tokens, location, &Location::setFastCgiPass);
}

void	locationHandleUpload(Location &location, std::vector<std::string> &tokens) {
	tokenSetter(tokens, location, &Location::setUpload);
}

void	locationHandleIndex(Location &location, std::vector<std::string> &tokens) {
	tokenSetter(tokens, location, &Location::setIndex);
}

void	locationHandleBodySize(Location &location, std::vector<std::string> &tokens) {
	tokenSetter(tokens, location, &Location::setBodySize);
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

void	locationHandleAutoIndex(Location &location, std::vector<std::string> &tokens) {
	tokenSetter(tokens, location, &Location::setAutoIndex);
}

typedef void (*locationHandler)(Location &, std::vector<std::string>&);

std::map<std::string, locationHandler> locationMap() {
	std::map<std::string, locationHandler> myMap;

	myMap["methods"] = &locationHandleMethods;
	myMap["root"] = &locationHandleRoot;
	myMap["cgi_bin"] = &locationHandleCgi;
	myMap["fastcgi_pass"] = &locationHandleFastCgiPass;
	myMap["upload"] = &locationHandleUpload;
	myMap["index"] = &locationHandleIndex;
	myMap["clientMaxBodySize"] = &locationHandleBodySize;
	myMap["return"] = &locationHandleReturn;
	myMap["access"] = &locationHandleAccess;
	myMap["autoindex"] = &locationHandleAutoIndex;

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
    _fast_cgi_pass(""),
	_upload(""),
	_index(""),
	_client_max_body_size(0),
	_return(std::make_pair(redirCode, redirPath)),
	_access(true),
	_autoindex(false),
	_allocated(true) {
}

Location::Location(std::vector<std::string> &tokens):
	_locationName(""),
	_methods(),
	_root(""),
	_cgi(""),
    _fast_cgi_pass(""),
	_upload(""),
	_index(""),
	_client_max_body_size(0),
	_return(std::make_pair(0, "")),
	_access(true),
	_autoindex(false),
	_allocated(false)
{
	std::map<std::string, locationHandler> map = locationMap();

	if (*tokens.begin() != "{") {
        std::cout << "Locations name: " << *tokens.begin() << std::endl;
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
    std::cout << "-------Locations name after: " << *tokens.begin() << std::endl;
}

// std::ostream &operator<<(std::ostream &os, Location const &ref) { {
// 	os << "  Location: " << ref.getLocationName() << std::endl
// 	    << "	Methods:";
// 	for (size_t i = 0; i <  ref.getMethods().size(); ++i) {
// 			os << " " << ref.getMethods()[i];
// 	}
//     os << std::endl;
// 	os << "	Root: " << ref.getRoot() << std::endl
// 	    << "	CGI: " << ref.getCgi() << std::endl
// 	    << "	Upload: " << ref.getUpload() << std::endl
// 	    << "	Index: " << ref.getIndex() << std::endl
// 	    << "	MaxBodySize: " << ref.getMaxBody() << std::endl
// 	    << "	Access: " << (ref.getAccess() ? "true" : "false") << std::endl
// 	    << "	AutoIndex: " << (ref.getAutoIndex() ? "true" : "false") << std::endl
// 	    << "	Return: " << ref.getReturn().first << " " << ref.getReturn().second << std::endl;
// }

std::ostream &operator<<(std::ostream &os, Location const &ref) {
    os << "  Location: " << ref.getLocationName() << std::endl;

    if (!ref.getMethods().empty()) {
        os << "	Methods:";
        for (size_t i = 0; i < ref.getMethods().size(); ++i) {
            os << " " << ref.getMethods()[i];
        }
        os << std::endl;
    }

    if (!ref.getRoot().empty()) {
        os << "	Root: " << ref.getRoot() << std::endl;
    }

    if (!ref.getCgi().empty()) {
        os << "	CGI: " << ref.getCgi() << std::endl;
    }

    if (!ref.getUpload().empty()) {
        os << "	Upload: " << ref.getUpload() << std::endl;
    }

    if (!ref.getIndex().empty()) {
        os << "	Index: " << ref.getIndex() << std::endl;
    }

    if (ref.getMaxBody() > 0) {
        os << "	MaxBodySize: " << ref.getMaxBody() << std::endl;
    }
    if (ref.getAccess() == false) {
        os << "	Access: " << "false" << std::endl;
    }
    if (ref.getAutoIndex() == true) {
        os << "	AutoIndex: " << "true" << std::endl;
    }

    if (ref.getReturn().first != 0 || !ref.getReturn().second.empty()) {
        os << "	Return: " << ref.getReturn().first << " " << ref.getReturn().second << std::endl;
    }

    return os;
}
