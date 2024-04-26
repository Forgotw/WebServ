/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsohler <lsohler@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 16:44:57 by lsohler           #+#    #+#             */
/*   Updated: 2024/04/26 15:35:42 by lsohler          ###   ########.fr       */
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

static std::string getContentType(const std::string& filename) {
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

void		findRequestLocation(t_response *response,t_data *data, Request const *request)
{
	if (data->routeFound.location.empty()) 
	{
		response->requestcode = 404;
		return;
	}
	// std::cout << path << std::endl;
	// std::cout << routeFound.root << std::endl;
	if (data->routeFound.access == false) {
		response->requestcode = 403;
		return;
	}
	if (!isAllowedMethod(data->routeFound, request->getMethod())) {
		response->requestcode = 405;
		return;
	}
	response->isDir = (data->path == data->location);
	if (response->isDir)
		response->pathToRespFile = data->routeFound.root;
	//std::cout << isDirectory << std::endl;
	if (response->isDir && !data->routeFound.listing && data->routeFound.index.empty()) {
		response->requestcode = 404;
		return;
	}
}

std::string getLastPathComponent(const std::string& path) {
    size_t lastSlashIndex = path.find_last_of('/');

    if (lastSlashIndex != std::string::npos && lastSlashIndex != path.length() - 1) {
        return path.substr(lastSlashIndex + 1);
    }
    return "";
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

void		findFilePath(t_response *response, t_data *data)
{
	std::string		file;
	if (!response->isDir)
		file = getLastPathComponent(data->path);
	else if (response->isDir && data->routeFound.index.empty())
	{
		response->list = true;
		return ;
	}
	else
		file = data->routeFound.index;
	if (!fileExistsInDirectory(data->routeFound.root, file))
		response->requestcode = 404;
	else
		response->pathToRespFile = data->routeFound.root + "/" + file;
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
    std::string	header;
	header += "HTTP/1.1 200 OK\r\n";
	header += "Content-Type: text/html\r\n";
	header += "\r\n";
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
            // if ((ent->d_type == DT_REG || ent->d_type == DT_DIR)
			// 	&& !(ent->d_name == ".." || ent->d_name == "."))
			if ((ent->d_type == DT_REG || ent->d_type == DT_DIR)
   				 && strcmp(ent->d_name, "..") != 0 && strcmp(ent->d_name, ".") != 0) {
                httpResponse += "<li>";
				httpResponse += "<a href=\"";
				httpResponse += ent->d_name;
				httpResponse += "\">";
                httpResponse += ent->d_name;
				httpResponse += "</a>";
                httpResponse += "</li>\n";
            }
        }
        closedir(dir);
    } else {
        return "";
    }

    httpResponse += "</ul>\n";
    httpResponse += "</body>\n";
    httpResponse += "</html>\n";

    return httpResponse;
}

std::string httpGetFormatter(unsigned int reqCode, std::string pathToFile)
{
    std::ifstream file(pathToFile, std::ios::binary);
    std::stringstream response;
    // Obtenir la taille du fichier
    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Allouer une chaîne de la taille du fichier
    std::string fileContent(fileSize, '\0');

    // Lire le contenu du fichier dans la chaîne
    file.read(&fileContent[0], fileSize);
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
    response << "Content-Type: " + getContentType(pathToFile) + "\r\n";
    response << "Content-Length: " <<  static_cast<long long int>(fileSize) << "\r\n";
    response << "\r\n";
    response << fileContent;

    return response.str();
}

std::string addTrailingSlashIfNoExtension(const std::string& str) {
    size_t dotIndex = str.find_last_of('.');
    size_t slashIndex = str.find_last_of('/');

    if (!str.empty() && str.back() == '/') {
        return str;
    }

    if (dotIndex == std::string::npos || (slashIndex != std::string::npos && dotIndex < slashIndex)) {
        return str + "/";
    } else {
        return str;
    }
}

void	fillDataStruct(t_data *data, Request const *request, Server const *serv)
{
	// data->path = addTrailingSlashIfNoExtension(request->getURI().path);
	// a fix 

	data->path = request->getURI().path;
	data->config = serv->getConfig();
	data->location = truncateStringAtLastSlash(data->path);

	std::cout << getLastPathComponent(data->path) <<std::endl;
	const std::map<std::string, Route>&				routes = data->config.getRoutes();
	std::map<std::string, Route>::const_iterator	it = routes.find(data->location);

	if (it != routes.end()) {
		data->routeFound = it->second;
	}
}

std::string treatRequest(Request const *request, Server const *serv)
{
	t_response		response;
	t_data			data;
	std::string		httpResponse = "";
	std::memset(&response, 0, sizeof(t_response));
	std::memset(&data, 0, sizeof(t_data));
	
	fillDataStruct(&data, request, serv);
	findRequestLocation(&response, &data, request);

	if (!response.requestcode)
		findFilePath(&response, &data);
	if (response.list)
	{
		std::cout << response.pathToRespFile << std::endl;
		httpResponse = handleListing(response.pathToRespFile);
		if (!httpResponse.empty())
			return httpResponse;
		else
			response.requestcode = 404;
	}
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