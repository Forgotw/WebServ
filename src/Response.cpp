/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsohler <lsohler@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 16:44:57 by lsohler           #+#    #+#             */
/*   Updated: 2024/05/03 21:22:39 by lsohler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

bool canOpen(const std::string& path) {
    std::ifstream file(path);
    if (file.is_open()) {
        file.close();
        return true;
    }
    return false;
}

static std::string searchFindReplace(std::string& toSearch, const std::string& toFind, const std::string& toReplace) {
    size_t pos = toSearch.find(toFind);
    if (pos == std::string::npos)
        return toSearch;

    toSearch.replace(pos, toFind.length(), toReplace);
    return toSearch;
}

std::string getContentType(const std::string& filename) {
    if (filename.find(".html") != std::string::npos ||
        filename.find(".htm") != std::string::npos) {
        return "text/html";
    }
    else if (filename.find(".css") != std::string::npos) {
        return "text/css";
    }
    else if (filename.find(".js") != std::string::npos) {
        return "application/javascript";
    }
    else if (filename.find(".xml") != std::string::npos) {
        return "application/xml";
    }
    else if (filename.find(".json") != std::string::npos) {
        return "application/json";
    }
    else if (filename.find(".jpg") != std::string::npos ||
             filename.find(".jpeg") != std::string::npos) {
        return "image/jpeg";
    }
    else if (filename.find(".png") != std::string::npos) {
        return "image/png";
    }
    else if (filename.find(".gif") != std::string::npos) {
        return "image/gif";
    }
    else if (filename.find(".ico") != std::string::npos) {
        return "image/x-icon";
    }
    else if (filename.find(".pdf") != std::string::npos) {
        return "application/pdf";
    }
    else if (filename.find(".txt") != std::string::npos) {
        return "text/plain";
    }
    else {
        return "application/octet-stream";
    }
}

std::string trimLastLoctation(std::string chaine) {
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

void	Response::splitSearchedURI(const std::string& input) {
	std::cout << "\n\nsplitSearchURI: " << input << "\n";
	if (input.find('.') != std::string::npos) {
		std::string::size_type lastSlashPos = input.rfind('/');

		std::cout << "Request URI: " << input << std::endl;
		//opendir()
		if (lastSlashPos != std::string::npos) {
			_searchedPage = input.substr(lastSlashPos + 1);
			_searchedLocation = input.substr(0, lastSlashPos + 1);
		} else {
			_searchedPage = "";
			_searchedLocation = input;
		}
		std::cout << "_searchedPage: " << _searchedPage << std::endl;
		std::cout << "_searchedLocation: " << _searchedLocation << std::endl;
	} else {
		_searchedPage = "";
		_searchedLocation = input;
	}
	std::cout << "_searchedPage: " << _searchedPage << std::endl;
	std::cout << "_searchedLocation: " << _searchedLocation << std::endl;
}

std::string getStringAfter(const std::string& str, const std::string& delimiter) {
    std::string::size_type pos = str.find(delimiter);

    if (pos != std::string::npos) {
        return str.substr(pos + delimiter.length());
    }

    return "";
}

bool				isAllowedMethod(Route routeFound, std::string method) {
	bool methodAllowed = false;
	for (size_t i = 0; i < routeFound.methods.size(); ++i) {
		if (routeFound.methods[i] == method) {
			methodAllowed = true;
			break;
		}
	}
	return methodAllowed;
}

unsigned int		Response::findLocation(void) {
	//-----------------recursive Search Location-----------------
	//
	const std::map<std::string, Route>&				routes = _config.getRoutes();
	unsigned int	redir = 0;
	std::string	searchedLocation = _searchedLocation;
	while (true) {
		std::map<std::string, Route>::const_iterator	it = routes.find(searchedLocation);
		if (it != routes.end()) {
			if (it->second._return.first != 0) {
				if (it->second._return.first == 404) {
					return 404;
				}
				std::cout << "\n\nRoute rediction found:\n";
				_config.printRoute(it->second);
				std::cout << "_route._return.second: " << _route._return.second << std::endl;
				_route = (routes.find(it->second._return.second))->second;
				redir = it->second._return.first;
				_config.printRoute(_route);
				_realPath = searchFindReplace(_searchedLocation, _route.location, _route.root);
				return 301;
			} else {
				_route = it->second;
				break;
			}
		} else {
			std::cout << "searchLocation 1: " << searchedLocation << std::endl;
			searchedLocation = trimLastLoctation(searchedLocation);
			std::cout << "searchLocation 2: " << searchedLocation << std::endl;
			it = routes.find(searchedLocation);
		}
		if (searchedLocation == "/" && routes.find(searchedLocation) == routes.end()) {
			return 404;
		}
	}
	//---------------------------------------------------
	_config.printRoute(_route);
	if (_route.access == false) {
		return 403;
	}
	if (!isAllowedMethod(_route, _request.getMethod())) {
		return 405;
	}
	if (_route.location != _searchedLocation) {
		_realPath = searchFindReplace(_searchedLocation, _route.location, _route.root) + _searchedPage;
		std::cout << "IF 1: " << _realPath << std::endl;
	} else {
		_realPath = _route.root + _searchedPage;
		std::cout << "ELSE: " << _realPath << std::endl;
	}
	DIR*	dir;

	dir = opendir(_realPath.c_str());
	if (dir == NULL) {
		_isDir = false;
		std::cout << "it is not dir\n";
	}
	else {
		_isDir = true;
		closedir(dir);
		if (_realPath[_realPath.size() - 1] != '/') {
			_realPath += '/';
		}
		if (!_route.listing) {
			_isDir = false;
			_realPath += _route.index;
		}
		std::cout << "_realPath(is dir): " << _realPath << std::endl;
	}
	if (_isDir && !_route.listing && _route.index.empty()) {
		_isDir = false;
		return 404;
	} else if (_isDir && !_route.listing && !_route.index.empty()) {
		_isDir = false;
		_searchedPage = _route.index;
	}
	if (redir)
		return redir;
	return 200;
}

void	Response::findErrorPage() {
	std::map<int, std::string>					error_page = _config.getErrorPage();
	std::map<int, std::string>::const_iterator	it = error_page.find(_returnCode);

	if (it != error_page.end()) {
		_realPath = _config.getRoot() + "default_error/" + it->second;
	} else {
		_realPath =  DEFAULT_ERROR_PAGE;
	}
}

void	Response::writeListingPage() {
	std::string httpResponse = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
    httpResponse += "<!DOCTYPE html>\n";
    httpResponse += "<html>\n";
    httpResponse += "<head>\n";
    httpResponse += "<title>File List</title>\n";
    httpResponse += "</head>\n";
    httpResponse += "<body>\n";
    httpResponse += "<h1>File List</h1>\n";
    httpResponse += "<ul>\n";

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(_realPath.c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
			if ((ent->d_type == DT_REG || ent->d_type == DT_DIR)
   				 && strcmp(ent->d_name, "..") != 0 && strcmp(ent->d_name, ".") != 0) {
                httpResponse += "<li>";
				httpResponse += "<a href=\"";
				httpResponse += ent->d_name;
				if (ent->d_type == DT_DIR) {
					httpResponse += "/";
				}
				httpResponse += "\">";
                httpResponse += ent->d_name;
				if (ent->d_type == DT_DIR) {
					httpResponse += "/";
				}
				httpResponse += "</a>";
                httpResponse += "</li>\n";
            }
        }
        closedir(dir);
    } else {
        return ;
    }
    httpResponse += "</ul>\n";
    httpResponse += "</body>\n";
    httpResponse += "</html>\n";
	_body = httpResponse;
}


void	Response::httpGetFormatter() {
	std::stringstream response;
		std::string htmlContent;
	if (_returnCode != 301) {
		std::ifstream file(_realPath);
		std::stringstream buffer;
		buffer << file.rdbuf();
		htmlContent = buffer.str();
		// Obtenir la taille du fichier
		file.seekg(0, std::ios::end);
		std::streampos fileSize = file.tellg();
		file.seekg(0, std::ios::beg);

		// Allouer une chaîne de la taille du fichier
		std::string fileContent(fileSize, '\0');
	    file.read(&fileContent[0], fileSize);
	}

    // Lire le contenu du fichier dans la chaîne
	std::cout << "httpGetFormater: " << _returnCode << " " << _realPath << " " << "_searchedLocation" << std::endl;
    response << "HTTP/1.1 " << _returnCode << " ";
    switch (_returnCode) {
        case 200:
            response << "OK";
            break;
		case 301:
			response << "Moved Permanently";
			break;
		case 400:
			response << "Bad Request";
			break;
		case 401:
			response << "Unauthorized";
			break;
		case 403:
			response << "Forbidden";
			break;
        case 404:
            response << "Not Found";
            break;
		case 405:
			response << "Method Not Allowed";
			break;
		case 500:
			response << "Internal Server Error";
			break;
		case 501:
			response << "Not Implemented";
			break;
		default:
			response << "Internal Server Error";
			break;
    }
	/*
	Content-Type: text/html
Content-Length: 169
Location: http://localhost:8081/dossier/
	*/
	if (_returnCode == 301) {
		response << "\r\n";
		response << "Location: /database/\r\n";
		response << "\r\n";
	} else {
		response << "\r\n";
		response << "Content-Type: " + getContentType(_realPath) + "\r\n";
		response << "Content-Length: " << htmlContent.length() << "\r\n";
		response << "\r\n";
	}
	_header = response.str();
	_body = htmlContent;
}

Response::Response(const ServerConfig &config, const Request &request) :
	_config(config),
	_request(request),
	_realPath(""),
	_searchedPage(""),
	_searchedLocation(""),
	_returnCode(0),
	_header(""),
	_body(""),
	_route() {
	std::cout << "New Response object\n";
	splitSearchedURI(_request.getURI().path);
	_returnCode = findLocation();
	std::cout << "_returnCode: " << _returnCode << std::endl;
	std::cout << "_isDir: " << _isDir << std::endl;
	if (_returnCode >= 400 || _returnCode == 0) {
		findErrorPage();
		httpGetFormatter();
	} else {
		if (_isDir) {
			std::cout << "IS DIR\n";
			writeListingPage();
		} else {
			std::cout << "IS not DIR\n";
			if (canOpen(_realPath) || _returnCode == 301) {
				std::cout << "CAN OPEN\n";
				httpGetFormatter();
			} else {
				_returnCode = 404;
				findErrorPage();
				std::cout << "CANNOT OPEN: " << _realPath << std::endl;
				httpGetFormatter();
			}
		}
	}
	std::cout << "New Response object finish\n";
}

bool	isListing(const Route* foundRoute, std::string responseFilePath) {
	struct stat	sb;
	if (stat(responseFilePath.c_str(), &sb) == -1) {
		throw std::runtime_error(std::string("stat: ") + std::strerror(errno));
	}
	//TODO: checker nginx
	if (S_ISDIR(sb.st_mode) && foundRoute->listing) {
		return true;
	}
	return false;
}
//			Response			response(foundRoute, responseFilePath, responseCode, request);
Response::Response(const Route* foundRoute, std::string responseFilePath, unsigned int responseCode, const Request& request) {
	(void)foundRoute;
	_request = request;

	_realPath = responseFilePath;
	_returnCode = responseCode;
	if (_returnCode == 301) {
		_header += "HTTP/1.1 301 Moved Permanently\r\n";
		_header += "Location: ";
		_header += foundRoute->_return.second;
		_header += "\r\n";
	} else if (isListing(foundRoute, responseFilePath)) {
		writeListingPage();
	} else {
		httpGetFormatter();
	}
}
