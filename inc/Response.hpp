/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsohler@student.42.fr <lsohler>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 11:05:57 by lsohler           #+#    #+#             */
/*   Updated: 2024/05/08 16:12:18 by lsohler@stu      ###   ########.fr       */
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
			std::string						_response;

			void			httpGetFormatter(const std::string& responseFilePath, unsigned int returnCode);
			void			writeListingPage(const std::string& responseFilePath);
			void			handleCGI(const Location* foundLocation, std::string responseFilePath, const Request& request);
			void			handleRedir(const Location* foundLocation);
	public:
		Response() {}
		Response(const ServerConfig &config, const Request &request);
		Response(const Location* foundLocation, std::string responseFilePath, unsigned int returnCode, const Request& request);
		Response(const std::string& Response);
		~Response() {}

		std::string getResponse() const { return _response;}

};
