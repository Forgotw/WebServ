/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsohler <lsohler@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 16:44:57 by lsohler           #+#    #+#             */
/*   Updated: 2024/06/08 18:44:09 by lsohler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdexcept>
#include <cstring>
#include <sys/uio.h>
#include "Server.hpp"
#include "Response.hpp"

typedef std::map<unsigned int, std::string> ErrorMap;

ErrorMap initializeErrorMap() {
    ErrorMap errorMap;

    errorMap[100] = "Continue";
    errorMap[101] = "Switching Protocols";
    errorMap[102] = "Processing"; // WebDAV; RFC 2518
    errorMap[103] = "Early Hints"; // RFC 8297

    errorMap[200] = "OK";
    errorMap[201] = "Created";
    errorMap[202] = "Accepted";
    errorMap[203] = "Non-Authoritative Information";
    errorMap[204] = "No Content";
    errorMap[205] = "Reset Content";
    errorMap[206] = "Partial Content";
    errorMap[207] = "Multi-Status"; // WebDAV; RFC 4918
    errorMap[208] = "Already Reported"; // WebDAV; RFC 5842
    errorMap[226] = "IM Used"; // RFC 3229

    errorMap[300] = "Multiple Choices";
    errorMap[301] = "Moved Permanently";
    errorMap[302] = "Found";
    errorMap[303] = "See Other";
    errorMap[304] = "Not Modified";
    errorMap[305] = "Use Proxy";
    errorMap[306] = "(Unused)";
    errorMap[307] = "Temporary Redirect";
    errorMap[308] = "Permanent Redirect"; // RFC 7538

    errorMap[400] = "Bad Request";
    errorMap[401] = "Unauthorized";
    errorMap[402] = "Payment Required";
    errorMap[403] = "Forbidden";
    errorMap[404] = "Not Found";
    errorMap[405] = "Method Not Allowed";
    errorMap[406] = "Not Acceptable";
    errorMap[407] = "Proxy Authentication Required";
    errorMap[408] = "Request Timeout";
    errorMap[409] = "Conflict";
    errorMap[410] = "Gone";
    errorMap[411] = "Length Required";
    errorMap[412] = "Precondition Failed";
    errorMap[413] = "Payload Too Large";
    errorMap[414] = "URI Too Long";
    errorMap[415] = "Unsupported Media Type";
    errorMap[416] = "Range Not Satisfiable";
    errorMap[417] = "Expectation Failed";
    errorMap[418] = "I'm a teapot"; // RFC 2324, RFC 7168
    errorMap[421] = "Misdirected Request"; // RFC 7540
    errorMap[422] = "Unprocessable Entity"; // WebDAV; RFC 4918
    errorMap[423] = "Locked"; // WebDAV; RFC 4918
    errorMap[424] = "Failed Dependency"; // WebDAV; RFC 4918
    errorMap[425] = "Too Early"; // RFC 8470
    errorMap[426] = "Upgrade Required";
    errorMap[428] = "Precondition Required"; // RFC 6585
    errorMap[429] = "Too Many Requests"; // RFC 6585
    errorMap[431] = "Request Header Fields Too Large"; // RFC 6585
    errorMap[451] = "Unavailable For Legal Reasons"; // RFC 7725

    errorMap[500] = "Internal Server Error";
    errorMap[501] = "Not Implemented";
    errorMap[502] = "Bad Gateway";
    errorMap[503] = "Service Unavailable";
    errorMap[504] = "Gateway Timeout";
    errorMap[505] = "HTTP Version Not Supported";
    errorMap[506] = "Variant Also Negotiates"; // RFC 2295
    errorMap[507] = "Insufficient Storage"; // WebDAV; RFC 4918
    errorMap[508] = "Loop Detected"; // WebDAV; RFC 5842
    errorMap[510] = "Not Extended"; // RFC 2774
    errorMap[511] = "Network Authentication Required"; // RFC 6585

    return errorMap;
}

static ErrorMap errorMap = initializeErrorMap();

std::string getErrorMessageHTML(unsigned int errorCode) {
    std::string errorMessage = "Unknown Error";
    if (errorMap.find(errorCode) != errorMap.end()) {
        errorMessage = errorMap[errorCode];
    }

    std::ostringstream oss;
    oss << "<html><head><title>Error " << errorCode << "</title></head>"
        << "<body><h1>Error " << errorCode << ": " << errorMessage << "</h1></body></html>";

    return oss.str();
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

std::string		Response::writeAutoIndexPage(const std::string& responseFilePath) {
	std::string httpResponse = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n";
	httpResponse += "<!DOCTYPE html>\n";
	httpResponse += "<html>\n";
	httpResponse += "<head>\n";
	httpResponse += "<title>File List</title>\n";
	httpResponse += "<link rel=\"stylesheet\" href=\"/style/listing.css\">\n";
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
					httpResponse += "\" class=\"dir\">";
				}
				else
					httpResponse += "\" class=\"jsp\">";
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
		return "";
	}
	httpResponse += "</ul>\n";
	httpResponse += "</body>\n";
	httpResponse += "</html>\n";
	return httpResponse;
}


std::string	Response::httpFormatter(const std::string& responseFilePath, unsigned int returnCode) {
	std::stringstream response;
	// // Obtenir la taille du fichier
	// file.seekg(0, std::ios::end);
	// std::streampos fileSize = file.tellg();
	// file.seekg(0, std::ios::beg);

	// // std::cout << "FILESIZE: " << fileSize << "\n";
	// // Allouer une chaîne de la taille du fichier
	// std::string fileContent(fileSize, '\0');
	// file.read(&fileContent[0], fileSize);

	// Lire le contenu du fichier dans la chaîne
	response << "HTTP/1.1 " << returnCode << " ";
    if (errorMap.find(returnCode) != errorMap.end()) {
        response << errorMap[returnCode];
    } else {
        response << "WebServ42 Custom Error";
    }
	response << "\r\n";
	std::string htmlContent;
	struct stat sb;
    if (!responseFilePath.empty() && stat(responseFilePath.c_str(), &sb) != -1) {
        std::ifstream file(responseFilePath.c_str());
        std::stringstream buffer;
        buffer << file.rdbuf();
        htmlContent = buffer.str();
        response << "Content-Type: " + getContentType(responseFilePath) + "\r\n";
        response << "Content-Length: " << htmlContent.length() << "\r\n";
    } else {
        htmlContent = getErrorMessageHTML(returnCode);
        response << "Content-Type: text/html\r\n";
        response << "Content-Length: " << htmlContent.length() << "\r\n";
        std::cout << "Default Error page generator for: " << std::to_string(returnCode) << std::endl;
    }
	//response << "Set-Cookie: SESSIONID=4242; Path=/; HttpOnly" << "\r\n";
	response << "\r\n";
	return (response.str() + htmlContent);
}

std::string		Response::handleRedir(const Location* foundLocation) {
	std::string response;
	response += "HTTP/1.1 301 Moved Permanently\r\n";
	response += "Location: ";
	response += foundLocation->getReturn().second;
	response += "\r\n";
	if (foundLocation->getAllocated()) {
		delete foundLocation;
	}
	return response;
}

std::string     Response::earlyErrorResponse(const Server* server, unsigned int error_code) {
    ServerConfig    config = server->getConfig();
    std::string errorFilePath = Server::generateReponseFilePath(error_code, "", server->getConfig());

	std::cout << "Early Response with code: " << error_code << std::endl;
    return httpFormatter(errorFilePath, error_code);
}
