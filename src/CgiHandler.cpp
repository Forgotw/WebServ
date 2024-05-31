/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsohler <lsohler@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 13:01:40 by lsohler           #+#    #+#             */
/*   Updated: 2024/05/31 13:09:14 by lsohler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiHandler.hpp"
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include <dirent.h>
#include <string>

bool isExecutable(const std::string& filePath) {
	struct stat fileInfo;
	if (stat(filePath.c_str(), &fileInfo) != 0) {
		return false;
	}
	if ((fileInfo.st_mode & S_IXUSR) != 0) {
		return true;
	}
	if ((fileInfo.st_mode & S_IXGRP) != 0) {
		return true;
	}
	if ((fileInfo.st_mode & S_IXOTH) != 0) {
		return true;
	}
	return false;
}

std::string	searchBinary(const std::string& cgiName) {
	std::string pathVariable = std::getenv("PATH");
	std::istringstream pathStream(pathVariable);
	std::string directory;

	while (std::getline(pathStream, directory, ':')) {
		DIR *dir = opendir(directory.c_str());
		if (dir != NULL) {
			struct dirent *entry;
			while ((entry = readdir(dir)) != NULL) {
				if (std::strcmp(entry->d_name, cgiName.c_str()) == 0) {
					closedir(dir);
					std::cout << "Real path for cgi: " << directory + "/" + cgiName << std::endl;
					return directory + "/" + cgiName;
				}
			}
			closedir(dir);
		}
	}
	return "";
}

unsigned int checkCgiError(const Location* location, std::string realPath, const Request& request) {
	(void)request;
	(void)realPath;
	std::string	cgiRealPath = searchBinary(location->getCgi());
	if (cgiRealPath.empty() || !isExecutable(cgiRealPath)) {
		return	500;
	}
	return 200;
}

char** generateEnvCgi(const Request& request, const ServerConfig* config, std::string realPath) {
	std::map<std::string, std::string> env;
	std::map<std::string, std::string> headers = request.getHeaders();

	if (!request.getBody().empty()) {
		std::stringstream ss;
		ss << request.getBody().size();
		env["CONTENT_LENGTH"] = ss.str();
	} else {
		env["CONTENT_LENGTH"] = "0";
	}
	env["CONTENT_TYPE"] = headers.count("Content-Type") ? headers["Content-Type"] : "application/x-www-form-urlencoded";
	env["GATEWAY_INTERFACE"] = "CGI/1.1";
	env["PATH_INFO"] = realPath;
	std::string querryString = request.getURI().querryString;
	if (!querryString.empty()) {
 		querryString = querryString.substr(1);
	} else {
		querryString = "";
	}
	env["QUERY_STRING"] = querryString;
	std::string host = headers["Host"];
	std::size_t pos = host.find(":");
	if (pos != std::string::npos) {
		host = host.substr(0, pos);
	}
	env["REMOTE_ADDR"] = host;
	env["REMOTE_HOST"] = headers["Host"];
	env["REQUEST_METHOD"] = request.getMethod();
	env["SCRIPT_NAME"] = request.getURI().path;
	env["SERVER_NAME"] = config->getHost();
	env["SERVER_PORT"] = config->getPort();
	env["TMPDIR"] = config->getUpload();
	env["SERVER_PROTOCOL"] = "HTTP/1.1";
	env["SERVER_SOFTWARE"] = "WebServ/1.0";
	env["SCRIPT_FILENAME"] = realPath;
	env["HTTP_ACCEPT"] = headers.count("Accept") ? headers["Accept"] : "";
	env["HTTP_ACCEPT_LANGUAGE"] = headers.count("Accept-Language") ? headers["Accept-Language"] : "";
	env["HTTP_USER_AGENT"] = headers.count("User-Agent") ? headers["User-Agent"] : "";
	env["HTTP_COOKIE"] = headers.count("Cookie") ? headers["Cookie"] : "";
	env["HTTP_REFERER"] = headers.count("Referer") ? headers["Referer"] : "";
	env["REDIRECT_STATUS"] = "200";

	char **envp = new char *[env.size() + 1];
	int i = 0;
	for (std::map<std::string, std::string>::iterator it = env.begin(); it != env.end(); ++it)
	{
		envp[i] = new char[it->second.size() + it->first.size() + 2];
		envp[i] = strcpy(envp[i], (it->first + "=" + it->second).c_str());
		i++;
	}
	envp[i] = NULL;
	return envp;
}

void	deleteEnv(char **env) {
	for (int i = 0; env[i]; i++) {
		delete env[i];
	}
	delete env;
}

void	printEnv(char **env) {
	std::cout << "PRINTING ENV\n";
	for (int i = 0; env[i]; i++) {
		std::cout << env[i] << "\n";
	}
}

std::string generateCgiResponse(const Location* foundLocation, std::string responseFilePath, const Request& request, const ServerConfig* config) {
    std::string response;
    std::string binary = searchBinary(foundLocation->getCgi());
    char* args[] = {&binary[0], &responseFilePath[0], NULL};
    char** envp = generateEnvCgi(request, config, responseFilePath);
    int pipefd[2];
    int stdinpipefd[2];
    if (pipe(pipefd) == -1 || pipe(stdinpipefd) == -1) {
        throw std::runtime_error(std::string("pipe: ") + std::strerror(errno));
    }
    pid_t pid = fork();
    if (pid == -1) {
        throw std::runtime_error(std::string("fork: ") + std::strerror(errno));
    }
    if (pid == 0) {
        if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
            throw std::runtime_error(std::string("dup2: ") + std::strerror(errno));
        }
        if (dup2(stdinpipefd[0], STDIN_FILENO) == -1) {
            throw std::runtime_error(std::string("dup2: ") + std::strerror(errno));
        }
		if (close(pipefd[0]) == -1 || close(pipefd[1]) == -1 || close(stdinpipefd[0]) == -1 || close(stdinpipefd[1]) == -1) {
			throw std::runtime_error(std::string("close: ") + std::strerror(errno));
		}
        execve(&binary[0], args, envp);
		throw std::runtime_error(std::string("execve: ") + std::strerror(errno));
    } else {
        int status;
		if (close(pipefd[1]) == -1 || close(stdinpipefd[0]) == -1) {
			throw std::runtime_error(std::string("close: ") + std::strerror(errno));
		}

        // Écrire le corps de la requête dans le descripteur d'écriture du tube

		if (request.getMethod() == "POST") {
			ssize_t writeResp = write(stdinpipefd[1], request.getBody().data(), request.getBody().size());
			if (writeResp == -1) {
				throw std::runtime_error(std::string("write: ") + std::strerror(errno));
			}
		}
		// Fermer le descripteur d'écriture après avoir écrit le corps de la requête
        if (close(stdinpipefd[1]) == -1) {
			throw std::runtime_error(std::string("close: ") + std::strerror(errno));
		}

        char buffer[BUFSIZ];
        int bytesRead;
        std::stringstream output;
		while (true) {
			bytesRead = read(pipefd[0], buffer, BUFSIZ);
			if (bytesRead < 0) {
				throw std::runtime_error(std::string("read: ") + std::strerror(errno));
			} else if (bytesRead == 0) {
				break;
			} else {
				output.write(buffer, bytesRead);
			}
		}
		if (close(pipefd[0]) == -1) {
			throw std::runtime_error(std::string("close: ") + std::strerror(errno));
		}
        std::string cgiOutput = output.str();
        response = cgiOutput;
        pid_t waitResp = waitpid(pid, &status, 0);
		if (waitResp == -1) {
			throw std::runtime_error(std::string("waitpid: ") + std::strerror(errno));
		}
    }
	deleteEnv(envp);
    return response;
}


std::string getCGIHeader(const std::string& respCGI) {
    std::size_t pos = respCGI.find("\r\n\r\n");
    if (pos != std::string::npos) {
        return respCGI.substr(0, pos + 4);
    }
    return "";
}

std::string getCGIContentType(const std::string& CGIHeader) {
    std::string contentType = "Content-Type: ";
    std::size_t startPos = CGIHeader.find(contentType);
    if (startPos != std::string::npos) {
        startPos += contentType.length();
        std::size_t endPos = CGIHeader.find("\r\n", startPos);
        if (endPos != std::string::npos) {
            std::string result = CGIHeader.substr(startPos, endPos - startPos);
            std::cout << "Found Content-Type: " << result << std::endl;  // Debug output
            return result;
        } else {
            std::cout << "End of line not found after Content-Type" << std::endl;  // Debug output
        }
    } else {
        std::cout << "Content-Type not found" << std::endl;  // Debug output
    }
    return "";
}

std::string getCGIStatusCode(const std::string& CGIHeader) {
    std::string status = "Status: ";
    std::size_t startPos = CGIHeader.find(status);
    if (startPos != std::string::npos) {
        startPos += status.length();
        std::size_t endPos = CGIHeader.find("\r\n", startPos);
        if (endPos != std::string::npos) {
            return CGIHeader.substr(startPos, endPos - startPos);
        }
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
    	return respCGI.substr(pos1); // +4 to skip over the second "\r\n\r\n"
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

std::string httpFormatterCGI(std::string contentType, std::string bodySize, std::string body, std::string location, std::string strStatusCode) {
	std::string response = "HTTP/1.1 ";
    unsigned int statusCode = std::atoi(strStatusCode.c_str());
	switch (statusCode) {
		case 200:
			response += "200 OK";
			break;
		case 201:
			response += "201 OK";
			break;
		case 302:
			response += "302 Found\r\n";
			response += "Location: ";
			response += location;
			response += "\r\n\r\n";
			return response;
		case 400:
			response += "400 Bad Request";
			break;
		case 404:
			response += "404 Not Found";
			break;
		default:
			response += "Internal Server Error";
			break;
	}
    response += "\r\n";
	response += "Content-Type: ";
	response += contentType;
	response += "\r\n";
	response += "Content-Length: ";
	response += bodySize;
	response += "\r\n";
	response += "\r\n";
	response += body;
	response += "\r\n";
    return response;
}

std::string	CgiHandler::handleCGI(unsigned int* uiStatusCode, const Location* foundLocation, std::string cgiFilePath, const Request& request, const ServerConfig* config) {
	std::string response;
	// std::cout << "Handle CGI\n";
	std::string respCGI = generateCgiResponse(foundLocation, cgiFilePath, request, config);
	// std::cout << "respCGI: " << respCGI << "\n";
	std::string CGIHeader = getCGIHeader(respCGI);
	// std::cout << "CGIHeader: " << CGIHeader << "\n";
	std::string contentType = getCGIContentType(CGIHeader);
	// std::cout << "contentType: " << contentType << "\n";
	std::string statusCode = getCGIStatusCode(CGIHeader);
	// std::cout << "statusCode: " << statusCode << "\n";
	std::string body = getCGIBody(respCGI);
	// std::cout << "body: " << body << "\n";
	std::string bodySize = getCGIBodySize(body);
	// std::cout << "bodySize: " << bodySize << "\n";
    std::string location = "";
	if (statusCode == "302") {
		location = getCGILocation(CGIHeader);
    }
	// if (statusCode == "200") {
	// 	std::string body = getCGIBody(respCGI);
	// 	std::string bodySize = getCGIBodySize(body);
	// 	response = handle200(contentType, bodySize, body);
	// 	std::cout << "response: " << response << "\n";
	// } else if (statusCode == "302") {
	// 	std::string location = getCGILocation(CGIHeader);
	// 	response = handle302(location);
	// } else {
	// 	*uiStatusCode = std::atoi(statusCode.c_str());
	// 	std::cout << "Handle CGI problem: " << *uiStatusCode << "\n";
	// }
    response = httpFormatterCGI(contentType, bodySize, body, location, statusCode);
    *uiStatusCode = 200; // TODO: handleCGI a ete refactor, may be on a plus besoin de cette variable, a discuter.
	return response;
}
