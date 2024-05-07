/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: efailla <efailla@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 11:05:57 by lsohler           #+#    #+#             */
/*   Updated: 2024/05/07 16:53:18 by efailla          ###   ########.fr       */
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
			ServerConfig					_config;
			Request							_request;
			std::string						_realPath;
			std::string						_searchedPage;
			std::string						_searchedLocation;
			unsigned int					_returnCode;
			std::string						_header;
			std::string						_body;
			// Route							_route;
			// Location						_location;
			bool							_isDir;

			void			splitSearchedURI(const std::string& input);
			void			httpGetFormatter();
			unsigned int	findLocation(void);
			void			findErrorPage(void);
			void			writeListingPage(void);
			unsigned int	recursiveSearchLocation(void);
			bool			handleCookies();

	public:
		Response() {}
		Response(const ServerConfig &config, const Request &request);
		Response(const Location* foundLocation, std::string responseFilePath, unsigned int responseCode, const Request& request);
		Response(const std::string& Response);
		~Response() {}

		std::string getResponse() const { return _header + _body;}

};
