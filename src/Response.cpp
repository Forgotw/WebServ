/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsohler <lsohler@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 16:44:57 by lsohler           #+#    #+#             */
/*   Updated: 2024/04/15 20:36:09 by lsohler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Peer.hpp"

bool canOpen(const std::string& path) {
    // Vérifier si le chemin existe
    if (pathExists(path)) {
        // Vérifier si on peut ouvrir le fichier ou le répertoire
        if (access(path.c_str(), R_OK | W_OK) == 0) {
            return true;
        }
    }
    
    return false;
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

std::string	treatRequestedFile(const ServerConfig& config, const Request& request) {
	std::string		requestedPath = request.getURI().path;
	if (requestedPath == "/") { // request Asking index
		return config.getRoot()+  "/" + config.getIndex();
	}

}

std::string truncateStringAtLastSlash(const std::string& input) {
    std::string::size_type lastSlashPos = input.rfind('/'); // Trouver la dernière position du slash

    if (lastSlashPos != std::string::npos) {
        return input.substr(0, lastSlashPos + 1); // Tronquer la chaîne jusqu'au dernier slash en incluant le slash
    }

    return input; // Retourner la chaîne originale si aucun slash n'est trouvé
}

std::string getStringAfter(const std::string& str, const std::string& delimiter) {
    std::string::size_type pos = str.find(delimiter);
    
    if (pos != std::string::npos) {
        // Extraire la sous-chaîne après le délimiteur
        return str.substr(pos + delimiter.length());
    }
    
    return ""; // Retourner une chaîne vide si le délimiteur n'est pas trouvé
}

bool				isAllowedMethod(Route routeFound, std::string method) {
	bool methodAllowed = false;
	for (size_t i = 0; i < routeFound.methods.size(); ++i) {
		if (routeFound.methods[i] == method) {
			methodAllowed = true;
			break;
		}
	}
}

unsigned int		Peer::findRequestLocation(void)
{
	std::string		path = _request->getURI().path;
	ServerConfig	config = _server->getConfig();
	std::string		location = truncateStringAtLastSlash(path);
	Route			routeFound;
	const std::map<std::string, Route>&				routes = config.getRoutes();
	std::map<std::string, Route>::const_iterator	it = routes.find(location);
	if (it != routes.end()) {
		routeFound = it->second;
	} else {
		return 404;
	}
	if (routeFound.access == false) {
		return 403;
	}
	if (!isAllowedMethod(routeFound, _request->getMethod())) {
		return 405;
	}
	bool isDirectory = (path == location);
	if (isDirectory && !routeFound.listing && routeFound.index.empty()) {
		return 404;
	}
	// fonction verifier acces au fichier, et peut etre dossier
	return 200;
}

// checker 


	// bool isDirectory = (path == location);
	// std::string	realPath;
	// if (isDirectory == false) {
		
	// }
	// realPath = routeFound.root + getStringAfter(path, location);
	// if (canOpen())