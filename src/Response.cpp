/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lray <lray@student.42lausanne.ch >         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 16:44:57 by lsohler           #+#    #+#             */
/*   Updated: 2024/05/11 17:56:59 by lray             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdexcept>
#include <cstring>
#include <sys/uio.h>


#include "Response.hpp"

std::string getCGIHeader(std::string respCGI);
std::string getCGIContentType(std::string CGIHeader);
std::string getCGIStatusCode(std::string CGIHeader);
std::string getCGILocation(std::string CGIHeader);
std::string getCGIBody(std::string respCGI);
std::string getCGIBodySize(std::string body);
std::string handle200(std::string type, std::string len, std::string body);
std::string handle302(std::string location);
std::string handle404(std::string type);
std::string handle500(std::string type);

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

void	Response::writeListingPage(const std::string& responseFilePath) {
	std::string httpResponse = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n";
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
	if ((dir = opendir(responseFilePath.c_str())) != NULL) {
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
	_response = httpResponse;
}


void	Response::httpGetFormatter(const std::string& responseFilePath, unsigned int returnCode) {
	std::stringstream response;
	std::string htmlContent;
	std::ifstream file(responseFilePath.c_str());
	std::stringstream buffer;
	buffer << file.rdbuf();
	htmlContent = buffer.str();
	// Obtenir la taille du fichier
	file.seekg(0, std::ios::end);
	std::streampos fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	std::cout << "FILESIZE: " << fileSize << "\n";
	// Allouer une chaîne de la taille du fichier
	std::string fileContent(fileSize, '\0');
	file.read(&fileContent[0], fileSize);

	// Lire le contenu du fichier dans la chaîne
	response << "HTTP/1.1 " << returnCode << " ";
	switch (returnCode) {
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
	response << "Content-Type: " + getContentType(responseFilePath) + "\r\n";
	response << "Content-Length: " << htmlContent.length() << "\r\n";
	response << "\r\n";
	_response = response.str() + htmlContent;
}

bool	isListing(const Location* foundLocation, std::string& responseFilePath) {
	struct stat	sb;
	if (stat(responseFilePath.c_str(), &sb) == -1) {
		throw std::runtime_error(std::string("stat: ") + std::strerror(errno));
	}
	if (S_ISDIR(sb.st_mode) && foundLocation->getListing() && foundLocation->getIndex().empty()) {
		return true;
	}
	return false;
}

void	Response::handleRedir(const Location* foundLocation) {
		_response += "HTTP/1.1 301 Moved Permanently\r\n";
		_response += "Location: ";
		_response += foundLocation->getReturn().second;
		_response += "\r\n";
		if (foundLocation->getAllocated()) {
			delete foundLocation;
		}
}

Response::Response(const Location* foundLocation, std::string responseFilePath, unsigned int returnCode, const Request& request, const ServerConfig* config) {
	if (foundLocation->isCgi() && returnCode == 200) {
		std::cout << "Handle CGI\n";
		std::string respCGI = handleCGI(foundLocation, responseFilePath, request, config);
		std::string CGIHeader = getCGIHeader(respCGI);
		std::string contentType = getCGIContentType(CGIHeader);
		std::string statusCode = getCGIStatusCode(CGIHeader);
		if (statusCode == "200") {
			std::string body = getCGIBody(respCGI);
			std::string bodySize = getCGIBodySize(body);
			_response = handle200(contentType, bodySize, body);
		} else if (statusCode == "302") {
			std::string location = getCGILocation(CGIHeader);
			_response = handle302(location);
		} else if (statusCode == "500") {
			_response = handle500(contentType);
		} else {
			_response = handle404(contentType);
		}
	} else if (returnCode == 301) {
		handleRedir(foundLocation);
	} else if (isListing(foundLocation, responseFilePath)) {
		writeListingPage(responseFilePath);
	} else {
		httpGetFormatter(responseFilePath, returnCode);
	}
}

std::string getCGIHeader(std::string respCGI) {
	std::size_t pos1 = respCGI.find("\r\n\r\n");
	if (pos1 != std::string::npos) {
		std::size_t pos2 = respCGI.find("\r\n\r\n", pos1 + 4);
		if (pos2 != std::string::npos) {
			return respCGI.substr(pos1 + 4, pos2 - pos1 - 4);
		}
	}
	return "";
}

std::string getCGIContentType(std::string CGIHeader) {
	std::string search = "Content-type:";
	std::size_t pos = CGIHeader.find(search);
	if (pos != std::string::npos) {
		std::string contentType = CGIHeader.substr(pos + search.length());

		std::size_t first = contentType.find_first_not_of(' ');
		if (first != std::string::npos) {
			contentType = contentType.substr(first);
		}
		std::size_t last = contentType.find_last_not_of("\r\n");
		if (last != std::string::npos) {
			contentType = contentType.substr(0, last + 1);
		}
		return contentType;
	}
	return "";
}

std::string getCGIStatusCode(std::string CGIHeader) {
	std::string search = "Status:";
	std::size_t pos = CGIHeader.find(search);
	if (pos != std::string::npos) {
		std::string status = CGIHeader.substr(pos + search.length());
		std::size_t first = status.find_first_not_of(' ');
		if (first != std::string::npos) {
			status = status.substr(first);
		}
		std::size_t end = status.find(' ');
		if (end != std::string::npos) {
			status = status.substr(0, end);
		}
		return status;
	}
	return "200";
}

std::string getCGILocation(std::string CGIHeader) {
	std::string search = "Location:";
	std::size_t pos = CGIHeader.find(search);
	if (pos != std::string::npos) {
		std::string location = CGIHeader.substr(pos + search.length());
		std::size_t first = location.find_first_not_of(' ');
		if (first != std::string::npos) {
			location = location.substr(first);
		}
		std::size_t end = location.find("\r\n");
		if (end != std::string::npos) {
			location = location.substr(0, end);
		}
		return location;
	}
	return "";
}

std::string getCGIBody(std::string respCGI) {
	std::size_t pos1 = respCGI.find("\r\n\r\n");
	if (pos1 != std::string::npos) {
		std::size_t pos2 = respCGI.find("\r\n\r\n", pos1 + 4); // +4 to skip over the first "\r\n\r\n"
		if (pos2 != std::string::npos) {
			return respCGI.substr(pos2 + 4); // +4 to skip over the second "\r\n\r\n"
		}
	}
	return "";
}

std::string getCGIBodySize(std::string body) {
	std::stringstream ss;
	ss << body.size();
	return ss.str();
}

std::string handle200(std::string type, std::string len, std::string body) {
	std::string resp = "";
	resp += "HTTP/1.1 200 OK";
	resp += "\r\n";
	resp += "Content-Type: ";
	resp += type;
	resp += "\r\n";
	resp += "Content-Length: ";
	resp += len;
	resp += "\r\n";
	resp += "\r\n";
	resp += body;
	resp += "\r\n";
	return resp;
}

std::string handle302(std::string location) {
	std::string resp = "";
	resp += "HTTP/1.1 302 Found\r\n";
	resp += "Location: ";
	resp += location;
	resp += "\r\n\r\n";
	return resp;
}

std::string handle404(std::string type) {
	std::string body = "";
	body += "<html>\r\n";
	body += "<head>\r\n";
	body += "<title>404 Not Found</title>\r\n";
	body += "</head>\r\n";
	body += "<body>\r\n";
	body += "<h1>404</h1>\r\n";
	body += "Not Found\r\n";
	body += "</body>\r\n";
	body += "</html>\r\n";

	std::string resp = "";
	resp += "HTTP/1.1 404 Not Found";
	resp += "\r\n";
	resp += "Content-Type: ";
	resp += type;
	resp += "\r\n";
	resp += "Content-Length: ";
	resp += body.size();
	resp += "\r\n\r\n";
	resp += body;
	return resp;
}

std::string handle500(std::string type) {
	std::string body = "";
	body += "<html>\r\n";
	body += "<head>\r\n";
	body += "<title>500 Internal Server Error</title>\r\n";
	body += "</head>\r\n";
	body += "<body>\r\n";
	body += "<h1>500</h1>\r\n";
	body += "Internal Server Error\r\n";
	body += "</body>\r\n";
	body += "</html>\r\n";

	std::string resp = "";
	resp += "HTTP/1.1 500 Internal Server Error";
	resp += "\r\n";
	resp += "Content-Type: ";
	resp += type;
	resp += "\r\n";
	resp += "Content-Length: ";
	resp += body.size();
	resp += "\r\n\r\n";
	resp += body;
	return resp;
}
