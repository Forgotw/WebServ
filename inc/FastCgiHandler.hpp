/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FastCgiHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsohler <lsohler@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/01 13:46:54 by lsohler           #+#    #+#             */
/*   Updated: 2024/06/02 11:41:07 by lsohler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

/*
**==========================
**		Include
**==========================
*/
#include "CgiHandler.hpp"

class FastCgiHandler {

    private:
        FastCgiHandler() {}
        ~FastCgiHandler() {}

    public:

        static std::string generateFastCgiResponse(char** envp, const std::string& fastcgi_pass, const Request& request);
        static std::string setFastCgiPass(const Location* foundLocation, const Location* cgiLocation);
        static bool        isPhpExtension(const std::string& path);
};
