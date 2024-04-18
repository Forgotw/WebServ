/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: efailla <efailla@42Lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 16:44:57 by lsohler           #+#    #+#             */
/*   Updated: 2024/04/18 22:27:54 by efailla          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Peer.hpp"
#include <unistd.h>
#include <dirent.h>

// bool canOpen(const std::string& path) {
//     // Vérifier si le chemin existe
//     if (pathExists(path)) {
//         // Vérifier si on peut ouvrir le fichier ou le répertoire
//         if (access(path.c_str(), R_OK | W_OK) == 0) {
//             return true;
//         }
//     }
    
//     return false;
// }

// std::string getContentType(const std::string& filename) {
//     if (filename.find(".html") != std::string::npos || 
//         filename.find(".htm") != std::string::npos) {
//         return "text/html";
//     } 
//     else if (filename.find(".css") != std::string::npos) {
//         return "text/css";
//     }
//     else if (filename.find(".js") != std::string::npos) {
//         return "application/javascript";
//     }
//     else if (filename.find(".xml") != std::string::npos) {
//         return "application/xml";
//     }
//     else if (filename.find(".json") != std::string::npos) {
//         return "application/json";
//     }
//     else if (filename.find(".jpg") != std::string::npos || 
//              filename.find(".jpeg") != std::string::npos) {
//         return "image/jpeg";
//     }
//     else if (filename.find(".png") != std::string::npos) {
//         return "image/png";
//     }
//     else if (filename.find(".gif") != std::string::npos) {
//         return "image/gif";
//     }
//     else if (filename.find(".ico") != std::string::npos) {
//         return "image/x-icon";
//     }
//     else if (filename.find(".pdf") != std::string::npos) {
//         return "application/pdf";
//     }
//     else if (filename.find(".txt") != std::string::npos) {
//         return "text/plain";
//     }
//     else {
//         return "application/octet-stream";
//     }
// }

std::string	treatRequestedFile(const ServerConfig& config, const Request& request) {
	std::string		requestedPath = request.getURI().path;
	if (requestedPath == "/") { // request Asking index
		return config.getRoot()+  "/" + config.getIndex();
	}
	return "fuck";
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
	return methodAllowed;
}

std::string apres_dernier_slash(const std::string& chemin)
{
    size_t pos_dernier_slash = chemin.find_last_of("/");
    if (pos_dernier_slash == std::string::npos) {
        return chemin;
    }
    return chemin.substr(pos_dernier_slash + 1);
}

// void		Peer::verifyFileAcess(bool isDirectory, Route& routeFound, t_response *response)
// {
// 	if (isDirectory)
// 	{
// 		std::string indexPath = routeFound.root + routeFound.index;
// 		if (access(indexPath.c_str(), R_OK) == 0)
// 		{
// 			response->requestcode = 200;
// 			response->pathToRespFile = indexPath;
// 			//renvoie index
// 		}
// 		else if (access(indexPath.c_str(), R_OK) == -1 && !routeFound.index.empty())
// 		{
// 			response->requestcode = 403;
			
// 		}
// 		else
// 		{
// 			response->requestcode = 200;
// 			response->list = true;
// 			//renvoie listing
// 		}
// 	}
// 		//verifie l'acces au fichier index du dossier/ sinon listing
// 	else
// 	{
// 		std::string filePath = routeFound.root + apres_dernier_slash(_request->getURI().path);
// 		if (access(filePath.c_str(), R_OK) == 0)
// 		{
// 			response->requestcode = 200;
// 			response->pathToRespFile = filePath;
// 		}	//renvoie fichier
// 		else
// 		{
// 			response->requestcode = 403;
// 		}
// 	}
// }

// void		Peer::findRequestLocation(t_response *response)
// {
// 	std::string		path = _request->getURI().path;
// 	ServerConfig	config = _server->getConfig();
// 	std::string		location = truncateStringAtLastSlash(path);
// 	Route			routeFound;
// 	const std::map<std::string, Route>&				routes = config.getRoutes();
// 	std::map<std::string, Route>::const_iterator	it = routes.find(location);
// 	if (it != routes.end()) {
// 		routeFound = it->second;
// 	} else {
// 		response->requestcode = 404;
// 		return;
// 	}
// 	if (routeFound.access == false) {
// 		response->requestcode = 403;
// 		return;
// 	}
// 	if (!isAllowedMethod(routeFound, _request->getMethod())) {
// 		response->requestcode = 405;
// 		return;
// 	}
// 	bool isDirectory = (path == location);
// 	if (isDirectory && !routeFound.listing && routeFound.index.empty()) {
// 		response->requestcode = 404;
// 		return;
// 	}
// 	verifyFileAcess(isDirectory, routeFound, response);
// }

// void		verifyFileAcess(bool isDirectory, Route& routeFound, t_response *response)
// {
// 	if (isDirectory)
// 	{
// 		std::string indexPath = routeFound.root + routeFound.index;
// 		if (access(indexPath.c_str(), R_OK) == 0)
// 		{
// 			response->requestcode = 200;
// 			response->pathToRespFile = indexPath;
// 			//renvoie index
// 		}
// 		else if (access(indexPath.c_str(), R_OK) == -1 && !routeFound.index.empty())
// 		{
// 			response->requestcode = 403;
			
// 		}
// 		else
// 		{
// 			response->requestcode = 200;
// 			response->list = true;
// 			//renvoie listing
// 		}
// 	}
// 		//verifie l'acces au fichier index du dossier/ sinon listing
// 	else
// 	{
// 		std::string filePath = routeFound.root + apres_dernier_slash(_request->getURI().path);
// 		if (access(filePath.c_str(), R_OK) == 0)
// 		{
// 			response->requestcode = 200;
// 			response->pathToRespFile = filePath;
// 		}	//renvoie fichier
// 		else
// 		{
// 			response->requestcode = 403;
// 		}
// 	}
// }

void		findRequestLocation(t_response *response,Request const *request, Server const *serv)
{
	std::string		path = request->getURI().path;
	ServerConfig	config = serv->getConfig();
	std::string		location = truncateStringAtLastSlash(path);
	Route			routeFound;
	const std::map<std::string, Route>&				routes = config.getRoutes();
	std::map<std::string, Route>::const_iterator	it = routes.find(location);

	if (it != routes.end()) {
		routeFound = it->second;
	} else {
		response->requestcode = 404;
		return;
	}
	// std::cout << path << std::endl;
	// std::cout << routeFound.root << std::endl;
	if (routeFound.access == false) {
		response->requestcode = 403;
		return;
	}
	if (!isAllowedMethod(routeFound, request->getMethod())) {
		response->requestcode = 405;
		return;
	}
	response->isDir = (path == location);
	if (response->isDir)
		response->pathToRespFile = routeFound.root;
	//std::cout << isDirectory << std::endl;
	if (response->isDir && !routeFound.listing && routeFound.index.empty()) {
		response->requestcode = 404;
		return;
	}
}

std::string getLastPathComponent(const std::string path)
{
    size_t lastSlashIndex = path.find_last_of('/');
    
	if (lastSlashIndex != std::string::npos && lastSlashIndex != path.length() - 1)
	{
        return path.substr(lastSlashIndex + 1);
    }
    return path;
}

bool fileExistsInDirectory(const std::string directoryPath, const std::string filename)
{
    std::string filePath = directoryPath + "/" + filename;
    std::ifstream file(filePath.c_str());
   
    if (file.is_open()) {
        file.close();
        return true;
    }
    return false;
}

void		findFilePath(t_response *response,Request const *request, Server const *serv)
{
	std::string		path = request->getURI().path;
	ServerConfig	config = serv->getConfig();
	std::string		location = truncateStringAtLastSlash(path);
	Route			routeFound;
	std::string		file;
	const std::map<std::string, Route>&				routes = config.getRoutes();
	std::map<std::string, Route>::const_iterator	it = routes.find(location);
	
	routeFound = it->second;

	if (!response->isDir)
		file = getLastPathComponent(path);
	else if (response->isDir && routeFound.index.empty())
	{
		response->list = true;
		return ;
	}
	else
		file = routeFound.index;
	if (!fileExistsInDirectory(routeFound.root, file))
		response->requestcode = 404;
	else
		response->pathToRespFile = routeFound.root + "/" + file;
}

void handleErrors(t_response *response) {
    if (response->requestcode != 200) {
        std::string defaultErrorsPath = "www/default_error/";
        std::string errorFileName = std::to_string(response->requestcode) + ".html";
        std::string errorFilePath = defaultErrorsPath + errorFileName;
        response->pathToRespFile = errorFilePath;
    }
}

std::string handleListing(const std::string& pathToDir) {
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
    if ((dir = opendir(pathToDir.c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG) { // Check if it's a regular file
                httpResponse += "<li>";
                httpResponse += ent->d_name;
                httpResponse += "</li>\n";
            }
        }
        closedir(dir);
    } else {
        return "HTTP/1.1 404 Not Found\r\n\r\nDirectory not found!";
    }

    httpResponse += "</ul>\n";
    httpResponse += "</body>\n";
    httpResponse += "</html>\n";

    return httpResponse;
}

std::string httpGetFormatter(unsigned int reqCode, std::string pathToFile)
{
    std::ifstream file(pathToFile);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string htmlContent = buffer.str();
    std::stringstream response;
    response << "HTTP/1.1 " << reqCode << " ";
    switch (reqCode) {
        case 200:
            response << "OK";
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
    response << "\r\n";
    response << "Content-Type: text/html\r\n";
    response << "Content-Length: " << htmlContent.length() << "\r\n";
    response << "\r\n";
    response << htmlContent;

    return response.str();
}

std::string treatRequest(Request const *request, Server const *serv)
{
	t_response		response;
	std::string		httpResponse = "";
	std::memset(&response, 0, sizeof(t_response));
	
	findRequestLocation(&response, request, serv);

	if (!response.requestcode)
		findFilePath(&response, request, serv);
	if (response.list)
	{
		std::cout << response.pathToRespFile << std::endl;
		httpResponse = handleListing(response.pathToRespFile);
		return httpResponse;
	}
	
	// if (request->getMethod() == "GET")
	// {
	// 	handleErrors(&response);
	// 	httpResponse = httpGetFormatter(response.requestcode, response.pathToRespFile);
	// }
	if (!response.requestcode)
		response.requestcode = 200;
	handleErrors(&response);
	httpResponse = httpGetFormatter(response.requestcode, response.pathToRespFile);
	return httpResponse;
}

// checker 


	// bool isDirectory = (path == location);
	// std::string	realPath;
	// if (isDirectory == false) {
		
	// }
	// realPath = routeFound.root + getStringAfter(path, location);
	// if (canOpen())