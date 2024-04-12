/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsohler <lsohler@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/06 18:52:20 by lsohler           #+#    #+#             */
/*   Updated: 2024/04/12 14:32:45 by lsohler          ###   ########.fr       */
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

struct	HTTPRequest;

struct URI {
	std::string scheme;		// Schéma (ex: "http", "https", etc.)
	std::string authority;	// Autorité (ex: "example.com")
	std::string path;		// Chemin (ex: "/index.html")
	std::string query;		// Requête (ex: "q=term")
	std::string fragment;	// Fragment (ex: "section2")
};

class Request {

	private:
		std::string							_method; // GET | POST | DELETE
		std::string							_version; // HTTP/1.1
		std::string							_rawURI;
		URI									_URI;
		std::map<std::string, std::string>	_headers;
		std::string							_body; // Juste pour POST et peut etre DELETE


	public:
		Request() : _method(), _version(), _rawURI(), _URI(), _headers() {}
		Request(const std::string& request);
		~Request() {}

};