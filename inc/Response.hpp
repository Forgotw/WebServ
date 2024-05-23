/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: efailla <efailla@42Lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 11:05:57 by lsohler           #+#    #+#             */
/*   Updated: 2024/05/23 10:48:21 by efailla          ###   ########.fr       */
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
};
