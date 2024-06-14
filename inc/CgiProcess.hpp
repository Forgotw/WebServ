/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiProcess.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsohler <lsohler@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/10 11:34:56 by lsohler           #+#    #+#             */
/*   Updated: 2024/06/11 15:50:48 by lsohler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

/*
**==========================
**		Include
**==========================
*/
#include "Request.hpp"
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cerrno>
#include <string>
#include <sstream>

#define CGI 1
#define FCGI 2

class CgiProcess {

	private:
        pid_t               _pid;
        int                 _pipefd[2];
        int                 _stdinpipefd[2];
        int                 _status;
        bool                _ready;
        bool                _readyToWrite;
        std::stringstream   _cgiOutput;
        int                 _type;
        int                 _sock;



	public:
        CgiProcess();
        CgiProcess(int type);
        ~CgiProcess() {}
        void    initCgiProcess(char* binary, char** args, char** envp, const Request& request);
        void    checkProcessStatus();
        void    writeCgiOuput();

        bool         isReady() { return _ready; }
        bool         isReadyToWrite() { return _readyToWrite; }
        void         setSocket(int socket) { _sock = socket; }
        std::string  getCgiOutputStr() { return _cgiOutput.str(); }
        int          getCgiPipe() { return _pipefd[0]; }
        int          getSocket() { return _sock; }

};
