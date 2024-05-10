/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsohler <lsohler@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/06 18:51:50 by lsohler           #+#    #+#             */
/*   Updated: 2024/05/10 17:17:10 by lsohler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include <iostream>
#include <fstream>

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

bool	isValidRequest(HTTPRequest httpRequest) {
	(void)httpRequest;
	return true;
}

Request::Request(const std::vector<char>& requestData) {
	// Convertir le vecteur de caractères en une chaîne de caractères
	std::string requestString(requestData.begin(), requestData.end());

	// Appeler le constructeur avec une chaîne de caractères
	*this = Request(requestString);
}

void Request::parseRequestLine(const std::string& requestLine) {
	// Analyser la ligne de requête (méthode, URI, version HTTP) et mettre à jour les attributs correspondants
	std::istringstream iss(requestLine);
	iss >> _method >> _rawURI >> _version;
}

void Request::parseHeaders(const std::string& headersData) {
	// Analyser les en-têtes de la requête et mettre à jour _headers
	// std::string headersData(requestData.begin() + startIdx, requestData.end());
	std::istringstream iss(headersData);
	std::string line;
	while (std::getline(iss, line) && !line.empty()) {
		size_t pos = line.find(":");
		if (pos != std::string::npos) {
			std::string key = line.substr(0, pos);
			std::string value = line.substr(pos + 1);
			_headers.insert(make_pair(key, value));
		}
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

// static void	testOnFile(std::string body, std::string boundary) {
//     std::size_t debut_binaire, fin_binaire;
//     if (body.empty() || boundary.empty()) {
// 		return ;
// 	}
//     // Cherche le début du binaire
//     debut_binaire = body.find(boundary);
//     if (debut_binaire == std::string::npos) {
//         std::cout << "Début du binaire non trouvé." << std::endl;
//         return;
//     }
//     debut_binaire = body.find("\r\n\r\n", debut_binaire);
//     if (debut_binaire == std::string::npos) {
//         std::cout << "Début du binaire non valide." << std::endl;
//         return;
//     }
//     debut_binaire += 4; // Saute les "\r\n\r\n"
    
//     // Cherche la fin du binaire
//     fin_binaire = body.find(boundary, debut_binaire);
//     if (fin_binaire - 1 == std::string::npos) {
//         std::cout << "Fin du binaire non trouvée." << std::endl;
//         return;
//     }
//     fin_binaire -= 2; // Retourne en arrière pour enlever "--" avant la délimitation
//     std::string donnees_binaires = body.substr(debut_binaire, fin_binaire - debut_binaire);
// 	std::ofstream fichier("request_binary", std::ios::binary);
// 	if (fichier.is_open()) {
// 		std::cout << "\n\n\n------------FICHIER WRITING------------\n";
// 		std::cout << "Lenght: " << donnees_binaires.length() << "\n";
// 		std::cout << "Size: " << donnees_binaires.size() << "\n";
// 		fichier.write(donnees_binaires.c_str(), donnees_binaires.size());
// 		std::cout << "\n------------FICHIER WRITING END------------\n";
// 	}
// }
Request::Request(const std::string& request) {
	std::istringstream iss(request);
	std::string requestLine;
	std::getline(iss, requestLine);

	// Analyser la ligne de requête
	parseRequestLine(requestLine);

	// Trouver la fin de l'en-tête
	size_t pos = request.find("\r\n\r\n");
	if (pos != std::string::npos) {
		// Analyser les en-têtes
		parseHeaders(request.substr(0, pos + 4)); // +4 pour sauter "\r\n\r\n"
		// Récupérer le corps de la requête
		_body = request.substr(pos + 4);
		// std::string contentType = (*_headers.find("Content-Type")).second;
		// std::string boundary = extractBoundary(contentType);
		// std::cout << contentType << "\n\n\n\n";
		// std::cout << boundary << "\n\n\n\n";
		// testOnFile(_body, boundary);
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
	// std::ofstream fichier("requestInPrint", std::ios::out | std::ios::binary);
	// if (fichier.is_open()) {
	// 	std::cout << "\n------------FICHIER WRITING------------\n";
	// 	std::cout << _body;
	// 	fichier.write(_body.c_str(), _body.length());
	// 	fichier.close();
	// }
}
