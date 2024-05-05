/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lray <lray@student.42lausanne.ch >         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 16:44:57 by lsohler           #+#    #+#             */
/*   Updated: 2024/05/05 13:16:15 by lray             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstring>

#include "Response.hpp"

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
	std::ifstream file(_realPath.c_str());
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
	response << "\r\n";
	response << "Content-Type: " + getContentType(_realPath) + "\r\n";
	response << "Content-Length: " << htmlContent.length() << "\r\n";
	response << "\r\n";
	_header = response.str();
	_body = htmlContent;
}

bool	isListing(const Location* foundLocation, std::string& responseFilePath) {
	struct stat	sb;
	if (stat(responseFilePath.c_str(), &sb) == -1) {
		throw std::runtime_error(std::string("stat: ") + std::strerror(errno));
	}
	//TODO: checker nginx
	if (S_ISDIR(sb.st_mode) && foundLocation->getListing() && foundLocation->getIndex().empty()) {
		return true;
	}
	return false;
}
//			Response			response(foundRoute, responseFilePath, responseCode, request);
Response::Response(const Location* foundLocation, std::string responseFilePath, unsigned int responseCode, const Request& request) {
	_request = request;

	_realPath = responseFilePath;
	_returnCode = responseCode;
	if (_returnCode == 301) {
		_header += "HTTP/1.1 301 Moved Permanently\r\n";
		_header += "Location: ";
		_header += foundLocation->getReturn().second;
		_header += "\r\n";
		if (foundLocation->getAllocated()) {
			delete foundLocation;
		}
	} else if (isListing(foundLocation, responseFilePath)) {
		writeListingPage();
	} else {
		httpGetFormatter();
	}
}
