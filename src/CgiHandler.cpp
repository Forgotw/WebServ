/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsohler <lsohler@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 13:01:40 by lsohler           #+#    #+#             */
/*   Updated: 2024/05/13 20:40:38 by lsohler          ###   ########.fr       */
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
	env["SERVER_NAME"] = config->getIP(); // FIXME: Il faudrait changer pour le hostname pour mettre server name si il y en a un
	env["SERVER_PORT"] = config->getPort();
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
	//printEnv(envp);
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
        response += "HTTP/1.1 200 OK\r\n\r\n";
        response += cgiOutput;
        pid_t waitResp = waitpid(pid, &status, 0);
		if (waitResp == -1) {
			throw std::runtime_error(std::string("waitpid: ") + std::strerror(errno));
		}
    }
	deleteEnv(envp);
    return response;
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

std::string	CgiHandler::handleCGI(unsigned int* uiStatusCode, const Location* foundLocation, std::string cgiFilePath, const Request& request, const ServerConfig* config) {
	std::string response;
	std::cout << "Handle CGI\n";
	std::string respCGI = generateCgiResponse(foundLocation, cgiFilePath, request, config);
	std::string CGIHeader = getCGIHeader(respCGI);
	std::string contentType = getCGIContentType(CGIHeader);
	std::string statusCode = getCGIStatusCode(CGIHeader);
	if (statusCode == "200") {
		std::string body = getCGIBody(respCGI);
		std::string bodySize = getCGIBodySize(body);
		response = handle200(contentType, bodySize, body);
	} else if (statusCode == "302") {
		std::string location = getCGILocation(CGIHeader);
		response = handle302(location);
	} else {
		*uiStatusCode = std::atoi(statusCode.c_str());
	}
	return response;
}
