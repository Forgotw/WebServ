/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsohler <lsohler@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/12 16:25:31 by lsohler           #+#    #+#             */
/*   Updated: 2024/06/01 11:21:38 by lsohler          ###   ########.fr       */
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
#include <vector>
#include <map>
#include <cstdlib>

#include <Location.hpp>

#define DEF_MAX_BODY_SIZE 10 * 1024 * 1024
#define DEFAULT_ERROR_PAGE "www/default_error/404.html"
#define CRITICAL_ERROR_PAGE "www/default_error/500.html"

unsigned int	maxBodySizeConverter(const std::string &size);

class ServerConfig {

	private:
		std::string						_ip;
		std::string						_port;
		std::string						_host;
		unsigned int					_client_max_body_size;
		std::string						_server_name;
		std::string						_access_log;
		std::string						_error_log;
		std::map<int, std::string>		_error_page;
		std::string						_error_dir;
		std::string						_root;
		std::string						_index;
		std::string						_upload;
		std::map<std::string, Location>	_locations;
		std::map<std::string, Location>	_cgiLocations;

	public:
		ServerConfig(void);
		ServerConfig(ServerConfig const &other);
		ServerConfig(std::vector<std::string> tokens);
		~ServerConfig(void);
		ServerConfig	&operator=(ServerConfig const &other);

		bool	isValidServerConfig();
		/*-----Set-----*/
		void	setIP(const std::string &ip) { _ip = ip; }
		void	setPort(const std::string &port) { _port = port; }
		void	setClientMaxBodySize(const std::string &size) { _client_max_body_size = maxBodySizeConverter(size); }
		void	setServerName(const std::string &name) { _server_name = name; }
		void	setAccessLog(const std::string &log) { _access_log = log; }
		void	setErrorLog(const std::string &log) { _error_log = log; }
		void	setErrorPage(const std::string &error) { _error_page.insert(std::make_pair(atoi(error.c_str()), error + ".html")); }
		void	setErrorDir(const std::string &dir) { _error_dir = dir; }
		void	setRoot(const std::string &root) { _root = root; }
		void	setIndex(const std::string &index) { _index = index; }
		void	setUpload(const std::string &upload) { _upload = upload; }
		void	setLocations(const Location &location) { _locations.insert(std::make_pair(location.getLocationName(), location)); }
		void	setCgiLocations(const Location &cgiLocation) { _cgiLocations.insert(std::make_pair(cgiLocation.getLocationName(), cgiLocation)); }

		/*-----Get-----*/
		const std::string&						getIP() const { return _ip; }
		const std::string&						getPort() const { return _port; }
		const std::string&						getHost() const { return _host; }
		unsigned int							getClientMaxBodySize() const { return _client_max_body_size; }
		const std::string&						getServerName() const { return _server_name; }
		const std::string&						getAccessLog() const { return _access_log; }
		const std::string&						getErrorLog() const { return _error_log; }
		const std::map<int, std::string>&		getErrorPage() const { return _error_page; }
		const std::string&						getErrorDir() const { return _error_dir; }
		const std::string&						getRoot() const { return _root; }
		const std::string&						getIndex() const { return _index; }
		const std::string&						getUpload() const { return _upload; }
		const std::map<std::string, Location>&	getLocations() const { return _locations; }
		const std::map<std::string, Location>&	getCgiLocations() const { return _cgiLocations; }
		/*-----Set Utils-----*/

		/*--------- Debugging --------*/
		void	printServerConfig(void) const;
};
