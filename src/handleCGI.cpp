/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleCGI.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsohler <lsohler@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 13:01:40 by lsohler           #+#    #+#             */
/*   Updated: 2024/05/09 19:30:25 by lsohler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>

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
/*
fastcgi_param  GATEWAY_INTERFACE  CGI/1.1;
fastcgi_param  SERVER_SOFTWARE    nginx;
fastcgi_param  QUERY_STRING       $query_string;
fastcgi_param  REQUEST_METHOD     $request_method;
fastcgi_param  CONTENT_TYPE       $content_type;
fastcgi_param  CONTENT_LENGTH     $content_length;
fastcgi_param  SCRIPT_FILENAME    $document_root$fastcgi_script_name;
fastcgi_param  SCRIPT_NAME        $fastcgi_script_name;
fastcgi_param  REQUEST_URI        $request_uri;
fastcgi_param  DOCUMENT_URI       $document_uri;
fastcgi_param  DOCUMENT_ROOT      $document_root;
fastcgi_param  SERVER_PROTOCOL    $server_protocol;
fastcgi_param  REMOTE_ADDR        $remote_addr;
fastcgi_param  REMOTE_PORT        $remote_port;
fastcgi_param  SERVER_ADDR        $server_addr;
fastcgi_param  SERVER_PORT        $server_port;
fastcgi_param  SERVER_NAME        $server_name;
      meta-variable-name =
						"AUTH_TYPE" |
						"CONTENT_LENGTH" |
                           "CONTENT_TYPE" |
						    "GATEWAY_INTERFACE" |
                           "PATH_INFO" |
                           "QUERY_STRING" |
						    "REMOTE_ADDR" |
                           "REMOTE_HOST" |
						    "REMOTE_IDENT" |
                           "REMOTE_USER" | 
						   "REQUEST_METHOD" |
                           "SCRIPT_NAME" | 
						   "SERVER_NAME" |
                           "SERVER_PORT" | 
						   "SERVER_PROTOCOL" |
                           "SERVER_SOFTWARE" |
						   scheme |
                           protocol-var-name |
						   extension-var-name
      protocol-var-name  = ( protocol | scheme ) "_" var-name
      scheme             = alpha *( alpha | digit | "+" | "-" | "." )
      var-name           = token
      extension-var-name = token
*/
char** generateEnvCgi(const Request& request, const ServerConfig* config, std::string realPath) {
	std::map<std::string, std::string> env;
	std::map<std::string, std::string> headers = request.getHeaders();

	env["SERVER_SOFTWARE"] = "WebServ/1.0";
	env["SERVER_NAME"] = config->getServerName();
	env["GATEWAY_INTERFACE"] = "CGI/1.1";

	env["SERVER_PROTOCOL"] = "HTTP/1.1";
	env["SERVER_PORT"] = config->getPort();
	env["REQUEST_METHOD"] = request.getMethod();
	env["PATH_INFO"] = request.getURI().pathInfo;
	env["QUERRY_STRING"] = request.getURI().querryString;
	env["SCRIPT_NAME"] = request.getURI().path;
	env["SCRIPT_FILENAME"] = "/Users/lsohler/WebServer/" + realPath;
	env["REMOTE_HOST"] = headers.count("host") ? headers["host"] : "";
	env["REMOTE_USER"] = headers.count("host") ? headers["host"] : "";
	env["CONTENT_TYPE"] = headers.count("content-length") ? headers["content-type"] : "";
	env["CONTENT_LENGTH"] = headers.count("content-length") ? headers["content-length"] : "";
	env["HTTP_ACCEPT"] = headers.count("Accept") ? headers["Accept"] : "";
	env["HTTP_ACCEPT_LANGUAGE"] = headers.count("Accept-Language") ? headers["Accept-Language"] : "";
	env["HTTP_USER_AGENT"] = headers.count("User-Agent") ? headers["User-Agent"] : "";
	env["HTTP_COOKIE"] = headers.count("Cookie") ? headers["Cookie"] : "";
	env["HTTP_REFERER"] = headers.count("Referer") ? headers["Referer"] : "";

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

// std::string	generateCgiResponse(const Location* foundLocation, std::string responseFilePath, const Request& request, const ServerConfig* config) {
// 	std::string response;
// 	std::string binary = searchBinary(foundLocation->getCgi());
// 	char* args[] = {&binary[0], &responseFilePath[0], NULL};
// 	char** envp = generateEnvCgi(request, config, responseFilePath);
// 	int pipefd[2];
// 	int	stdinpipefd[2];
// 	if (pipe(pipefd) == -1 || pipe(stdinpipefd) == -1) {
// 		throw std::runtime_error(std::string("pipe: ") + std::strerror(errno));
// 	}
// 	pid_t pid = fork();
// 	if (pid == -1) {
// 		throw std::runtime_error(std::string("fork: ") + std::strerror(errno));
// 	}
// 	if (pid == 0) {
// 		if (dup2(stdinpipefd[0], STDIN_FILENO) == -1) {
// 			throw std::runtime_error(std::string("dup2: ") + std::strerror(errno));
// 		}
// 		if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
// 			throw std::runtime_error(std::string("dup2: ") + std::strerror(errno));
// 		}
// 		close(pipefd[0]);
// 		close(pipefd[1]);
// 		close(stdinpipefd[0]);
// 		write(stdinpipefd[1], request.getBody().c_str(), request.getBody().size());
// 		close(stdinpipefd[1]);

// 		// std::cerr << "START OF BODY\n" << request.getBody() << "END OF BODY\n";
// 		execve(&binary[0], args, envp);
// 		throw std::runtime_error(std::string("execve: ") + std::strerror(errno));
// 	} else {
// 		int status;
// 		close(pipefd[1]);

//         // Écrire le corps de la requête dans le descripteur d'écriture du tube
// 		char buffer[BUFSIZ];
// 		int bytesRead;
// 		std::stringstream output;
// 		while ((bytesRead = read(pipefd[0], buffer, BUFSIZ)) > 0) {
// 			output.write(buffer, bytesRead);
// 		}
// 		close(pipefd[0]);
// 		// std::cout << "RESPONSE\n" << output.str() << "END OF RESPONSE\n";
// 		std::string cgiOutput = output.str();
// 		response += "HTTP/1.1 200 OK\r\n\r\n";
// 		response += cgiOutput;
// 		waitpid(pid, &status, 0);
// 	}
// 	std::cout << "RESPONSE\n" << response << "END OF RESPONSE\n";
// 	return response;
// }

// std::string generateCgiResponse(const Location* foundLocation, std::string responseFilePath, const Request& request, const ServerConfig* config) {
//     std::string response;
//     std::string binary = searchBinary(foundLocation->getCgi());
//     char* args[] = {&binary[0], &responseFilePath[0], NULL};
//     char** envp = generateEnvCgi(request, config, responseFilePath);
//     int pipefd[2];
//     if (pipe(pipefd) == -1) {
//         throw std::runtime_error(std::string("pipe: ") + std::strerror(errno));
//     }
//     pid_t pid = fork();
//     if (pid == -1) {
//         throw std::runtime_error(std::string("fork: ") + std::strerror(errno));
//     }
//     if (pid == 0) {
//         if (dup2(pipefd[0], STDIN_FILENO) == -1) {
//             throw std::runtime_error(std::string("dup2: ") + std::strerror(errno));
//         }
//         close(pipefd[0]);
//         close(pipefd[1]);
//         execve(&binary[0], args, envp);
//         throw std::runtime_error(std::string("execve: ") + std::strerror(errno));
//     } else {
//         int status;
//         close(pipefd[0]);
//         // Écrire le corps de la requête dans le descripteur d'écriture du tube
//         write(pipefd[1], request.getBody().c_str(), request.getBody().size());
//         close(pipefd[1]); // Fermer le descripteur d'écriture après avoir écrit le corps de la requête
//         waitpid(pid, &status, 0);
//         // Lire la réponse du fichier temporaire
// 		response += "HTTP/1.1 200 OK\r\n\r\n";
//         std::ifstream responseFile(responseFilePath);
//         if (responseFile) {
//             std::ostringstream oss;
//             oss << responseFile.rdbuf();
//             response += oss.str();
//         } else {
//             throw std::runtime_error("Failed to open response file");
//         }
//     }
//     return response;
// }

std::string generateCgiResponse(const Location* foundLocation, std::string responseFilePath, const Request& request, const ServerConfig* config) {
    std::string response;
    std::string binary = searchBinary(foundLocation->getCgi());
    char* args[] = {&binary[0], &responseFilePath[0], NULL};
    char** envp = generateEnvCgi(request, config, responseFilePath);
	printEnv(envp);
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
        close(pipefd[0]);
        close(pipefd[1]);
        close(stdinpipefd[0]);
        close(stdinpipefd[1]);

        execve(&binary[0], args, envp);
        throw std::runtime_error(std::string("execve: ") + std::strerror(errno));
    } else {
        int status;
        close(pipefd[1]);
        close(stdinpipefd[0]);
        close(stdinpipefd[1]);
        // Écrire le corps de la requête dans le descripteur d'écriture du tube
        write(stdinpipefd[1], request.getBody().c_str(), request.getBody().size());
        close(stdinpipefd[1]); // Fermer le descripteur d'écriture après avoir écrit le corps de la requête

        char buffer[BUFSIZ];
        int bytesRead;
        std::stringstream output;
        while ((bytesRead = read(pipefd[0], buffer, BUFSIZ)) > 0) {
            output.write(buffer, bytesRead);
        }
        close(pipefd[0]);
        std::string cgiOutput = output.str();
        response += "HTTP/1.1 200 OK\r\n\r\n";
        response += cgiOutput;
        waitpid(pid, &status, 0);
    }
	deleteEnv(envp);
    return response;
}



std::string	handleCGI(const Location* foundLocation, std::string responseFilePath, const Request& request, const ServerConfig* config) {
	return generateCgiResponse(foundLocation, responseFilePath, request, config);
}