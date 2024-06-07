/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsohler <lsohler@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/06 18:51:50 by lsohler           #+#    #+#             */
/*   Updated: 2024/06/02 15:12:18 by lsohler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include <iostream>
#include <fstream>

const char* HTTP_METHODS[] = {
	"OPTIONS",
	"GET",
	"HEAD",
	"POST",
	"PUT",
	"DELETE",
	"TRACE",
	"CONNECT"
};

bool isInHTTPMethods(const std::string& method) {
	for (size_t i = 0; i < sizeof(HTTP_METHODS) / sizeof(HTTP_METHODS[0]); ++i) {
		if (method == HTTP_METHODS[i]) {
			return true;
		}
	}
	return false;
}

bool	Request::isValidRequest() const {
	if (!isInHTTPMethods(_method)) {
		return false;
	}
	if (_rawURI.empty()) {
		return false;
	}
	if (_version != "HTTP/1.1") {
		return false;
	}
	return true;
}

URI parseURI(const std::string &uriStringConst) {
	std::string uriString = uriStringConst;
	URI uri;

	size_t querryStart = uriString.find("?");
	if (querryStart != std::string::npos) {
		uri.querryString = uriString.substr(querryStart);
		uriString = uriString.substr(0, querryStart);
	}
	size_t extensionStart = uriString.find(".");
	size_t pathInfoStart;
	if (extensionStart != std::string::npos) {
		pathInfoStart = uriString.find("/", extensionStart);
		if (pathInfoStart != std::string::npos) {
			uri.pathInfo = uriString.substr(pathInfoStart);
			uriString = uriString.substr(0, pathInfoStart);
		}
	}
	uri.path = uriString;
	return uri;
}


void Request::parseRequestLine(const std::string& requestLine) {
	std::istringstream iss(requestLine);
	iss >> _method >> _rawURI >> _version;
}

void Request::parseHeaders(const std::string& headersData) {
	std::istringstream iss(headersData);
	std::string line;
	while (std::getline(iss, line) && !line.empty()) {
		size_t pos = line.find(":");
		if (pos != std::string::npos) {
			std::string key = line.substr(0, pos);
			std::string value = line.substr(pos + 2);
			value.erase(value.size() - 1);
			_headers.insert(make_pair(key, value));
		}
	}
}
/*Voir RFC 7.2.1 Multipart: The common syntax*/
static void testOnFile(std::string body, const std::string& boundary) {
	std::size_t bin_start, bin_end;
	if (body.empty() || boundary.empty()) {
		return;
	}
	std::string boundary_start = "--" + boundary;
	std::string boundary_end = "--" + boundary + "--";
	std::cout << "boundary_start:    " << boundary_start << std::endl;
	std::cout << "boundary_end:      " << boundary_end << std::endl;
	bin_start = body.find(boundary);
		if (bin_start == std::string::npos) {
		std::cout << "Début du binaire non trouvé." << std::endl;
		return;
	}

	bin_start = body.find("\r\n\r\n", bin_start);
	if (bin_start == std::string::npos) {
		std::cout << "Début du binaire non valide." << std::endl;
		return;
	}

	bin_start += 4; // Saute les "\r\n\r\n"

	bin_end = body.find(boundary_end, bin_start);
	if (bin_end == std::string::npos) {
		std::cout << "Fin du binaire non trouvée." << std::endl;
		return;
	}

	std::string binary_data = body.substr(bin_start, bin_end - bin_start - 2);
	std::ofstream file("request_binary", std::ios::binary);
	if (file.is_open()) {
		std::cout << "\n\n\n------------ECRITURE DU FICHIER------------\n";
		std::cout << "Longueur: " << binary_data.length() << "\n";
		std::cout << "Taille: " << binary_data.size() << "\n";
		file.write(binary_data.c_str(), binary_data.size());
		std::cout << "\n------------FIN DE L'ÉCRITURE DU FICHIER------------\n";
	}
}

std::string extractBoundary(const std::string& en_tete) {
	std::string boundary;
	std::size_t pos_debut = en_tete.find("boundary=");

	if (pos_debut != std::string::npos) {
		boundary = en_tete.substr(pos_debut + 9, en_tete.length());
	}
	return boundary;
}

Request::Request(const std::string& request) : _method(""), _version(""), _rawURI(""), _URI(), _headers(), _body("") {
	std::istringstream iss(request);
	std::string requestLine;
	std::getline(iss, requestLine);

	parseRequestLine(requestLine);

	size_t pos = request.find("\r\n\r\n");
	if (pos != std::string::npos) {
		parseHeaders(request.substr(0, pos + 4));
		// printRequest();
		_body = request.substr(pos + 4);
		// std::cout << "Body: " << _body << std::endl;
		bool debug = false;
		if (debug && !_body.empty()) {
			std::cout << "Before\n";
			std::string contentType = (*_headers.find("Content-Type")).second;
			std::cout << "contenType: " << contentType << "\n";
			std::string boundary = extractBoundary(contentType);
			std::cout << "boundary:          " << boundary << "\n";
			std::cout << "After\n";
			testOnFile(_body, boundary);
			std::cout << "After2\n";
		}
	}
	_URI = parseURI(_rawURI);
}

void Request::printRequest() const {
	std::cout << "Method: " << _method << std::endl;
	std::cout << "Version: " << _version << std::endl;
	std::cout << "Raw URI: " << _rawURI << std::endl;
	std::cout << "URI Path: " << _URI.path << std::endl;
	std::cout << "URI PathInfo: " << _URI.pathInfo << std::endl;
	std::cout << "URI Querry: " << _URI.querryString << std::endl;
	std::cout << "Headers:" << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it) {
		std::cout << "    " << it->first << ": " << it->second << std::endl;
	}
    std::cout << "Body: " << _body << std::endl;
}
