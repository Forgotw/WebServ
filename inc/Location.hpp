/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: efailla <efailla@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 11:24:57 by lsohler           #+#    #+#             */
/*   Updated: 2024/05/14 15:53:26 by efailla          ###   ########.fr       */
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
#include <unistd.h>

class Location {

	private:

		std::string								_locationName;
		std::vector<std::string>				_methods;
		std::string								_root;
		std::string								_cgi;
		std::string								_upload;
		std::string								_index;
		std::pair<unsigned int, std::string>	_return;
		bool									_access;
		bool									_autoindex;
		bool									_allocated;

	public:

		Location(void);
		Location(unsigned int redirCode, std::string redirPath);
		Location(const Location& other);
		Location(std::vector<std::string> &tokens);
		~Location(void);
		Location	&operator=(const Location& other);
	
		/*-----Set-----*/
		void setLocationName(const std::string& location) { _locationName = location; }
		void setMethods(const std::string& method) { _methods.push_back(method); }
		void setRoot(const std::string& root) { _root = root; }
		void setCgi(const std::string& cgi) { _cgi = cgi; }
		void setUpload(const std::string& upload) { _upload = upload; }
		void setIndex(const std::string& index) { _index = index; }
		void setReturn(const std::pair<unsigned int, std::string>& ret) { _return = ret; }
		void setAccess(const std::string& access) { _access = true; if (access == "false") _access = false; }
		void setAutoIndex(const std::string& autoindex) { _autoindex = true; if (autoindex == "false") _autoindex = false; }

		/*-----Get-----*/
		const std::string& getLocationName() const { return _locationName; }
		const std::vector<std::string>& getMethods() const { return _methods; }
		const std::string& getRoot() const { return _root; }
		const std::string& getCgi() const { return _cgi; }
		const std::string& getUpload() const { return _upload; }
		const std::string& getIndex() const { return _index; }
		const std::pair<unsigned int, std::string>& getReturn() const { return _return; }
		bool getAccess() const { return _access; }
		bool getAutoIndex() const { return _autoindex; }
		bool getAllocated() const { return _allocated; }

		/*------------*/
		bool isCgi(void) const { return !_cgi.empty(); }
		/*-----Debug-----*/
		void	printLocation(void) const;
};
