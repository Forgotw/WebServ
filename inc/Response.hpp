/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsohler <lsohler@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 11:05:57 by lsohler           #+#    #+#             */
/*   Updated: 2024/05/07 17:05:03 by lsohler          ###   ########.fr       */
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

			void			splitSearchedURI(const std::string& input);
			void			httpGetFormatter(const std::string& responseFilePath, unsigned int returnCode);
			unsigned int	findLocation(void);
			void			findErrorPage(void);
			void			writeListingPage(const std::string& responseFilePath);
			unsigned int	recursiveSearchLocation(void);
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
