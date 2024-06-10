/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiProcess.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsohler <lsohler@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/10 11:34:38 by lsohler           #+#    #+#             */
/*   Updated: 2024/06/10 17:32:54 by lsohler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiProcess.hpp"


CgiProcess::CgiProcess() : 
    _pid(),
    _pipefd(), _stdinpipefd(),
    _status(), _ready(false),
    _readyToWrite(false),
    _cgiOutput(),
    _type(0) {
}

CgiProcess::CgiProcess(int type) : 
    _pid(),
    _pipefd(), _stdinpipefd(),
    _status(), _ready(false),
    _readyToWrite(false),
    _cgiOutput(),
    _type(type) {
}

void    CgiProcess::initCgiProcess(char* binary, char** args, char** envp, const Request& request) {

	if (pipe(_pipefd) == -1 || pipe(_stdinpipefd) == -1) {
		throw std::runtime_error(std::string("pipe: ") + std::strerror(errno));
	}
	_pid = fork();
	if (_pid == -1) {
		throw std::runtime_error(std::string("fork: ") + std::strerror(errno));
	}
	if (_pid == 0) {
		if (dup2(_pipefd[1], STDOUT_FILENO) == -1) {
			throw std::runtime_error(std::string("dup2: ") +
									 std::strerror(errno));
		}
		if (dup2(_stdinpipefd[0], STDIN_FILENO) == -1) {
			throw std::runtime_error(std::string("dup2: ") +
									 std::strerror(errno));
		}
		if (close(_pipefd[0]) == -1 || close(_pipefd[1]) == -1 ||
			close(_stdinpipefd[0]) == -1 || close(_stdinpipefd[1]) == -1) {
			throw std::runtime_error(std::string("close: ") +
									 std::strerror(errno));
		}
		execve(binary, args, envp);
		throw std::runtime_error(std::string("execve: ") +
								 std::strerror(errno));
	} else {
        if (close(_pipefd[1]) == -1 || close(_stdinpipefd[0]) == -1) {
            throw std::runtime_error(std::string("close: ") +
                                     std::strerror(errno));
        }

        if (request.getMethod() == "POST") {
            ssize_t writeResp = write(_stdinpipefd[1], request.getBody().data(),
                                      request.getBody().size());
            if (writeResp == -1) {
                throw std::runtime_error(std::string("write: ") +
                                         std::strerror(errno));
            }
        }
        if (close(_stdinpipefd[1]) == -1) {
            throw std::runtime_error(std::string("close: ") +
                                     std::strerror(errno));
        }
        checkProcessStatus();
    }
}

void    CgiProcess::checkProcessStatus() {
    if (_readyToWrite) {
        return ;
    }
    pid_t waitResp = waitpid(_pid, &_status, WNOHANG);
    if (waitResp == -1) {
        throw std::runtime_error(std::string("waitpid: ") + std::strerror(errno));
    }
    if (waitResp == 0) {
        return ;
    } else {
        if (WIFEXITED(_status) || WIFSIGNALED(_status)) {
            std::cout << "ReadyToWrite\n";
            _readyToWrite = true;
            return ;
        }
    }
}


void    CgiProcess::writeCgiOuput() {
    char buffer[BUFSIZ];
    int bytesRead;
    (void)_type;

    std::cout << "writeCgiOutput\n";
    bytesRead = read(_pipefd[0], buffer, BUFSIZ);
    if (bytesRead < 0) {
        throw std::runtime_error(std::string("read: ") +
                                    std::strerror(errno));
    } else if (bytesRead == 0) {
        if (close(_pipefd[0]) == -1) {
            throw std::runtime_error(std::string("close: ") +
                                        std::strerror(errno));
        } else {
            std::cout << "cgiOutput is Ready\n";
            _ready = true;
        }
    } else {
        _cgiOutput.write(buffer, bytesRead);
    }
}