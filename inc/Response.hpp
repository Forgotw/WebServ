/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsohler <lsohler@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 11:05:57 by lsohler           #+#    #+#             */
/*   Updated: 2024/06/03 15:22:54 by lsohler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#pragma once

/*
**==========================
**		Include
**==========================
*/
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <fstream>
#include <ServerConfig.hpp>
#include <Request.hpp>
#include <dirent.h>
#include <unistd.h>

class Response {

	private:
		Response() {}
		~Response() {}

	public:
		static std::string		httpFormatter(const std::string& responseFilePath, unsigned int returnCode);
		static std::string		writeAutoIndexPage(const std::string& responseFilePath);
		static std::string		handleRedir(const Location* foundLocation);
        static std::string      earlyErrorResponse(const Server* server, unsigned int error_code);
};
