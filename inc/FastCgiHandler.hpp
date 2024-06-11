/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FastCgiHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsohler <lsohler@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/01 13:46:54 by lsohler           #+#    #+#             */
/*   Updated: 2024/06/11 15:43:50 by lsohler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

/*
**==========================
**		Include
**==========================
*/
#include "CgiHandler.hpp"
#include "CgiProcess.hpp"

class FastCgiHandler {

    private:
        FastCgiHandler() {}
        ~FastCgiHandler() {}

    public:

        // static std::string generateFastCgiResponse(char** envp, const std::string& fastcgi_pass, const Request& request);
        static void handleFastCGIRequest(const std::string& fastcgi_pass, const std::map<std::string, std::string>& envp, const std::string& body, CgiProcess& cgi_process);
        static std::string setFastCgiPass(const Location* foundLocation, const Location* cgiLocation);
        static bool        isPhpExtension(const std::string& path);
};
