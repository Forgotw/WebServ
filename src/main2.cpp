/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsohler <lsohler@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/11 15:54:52 by lsohler           #+#    #+#             */
/*   Updated: 2024/04/12 15:22:26 by lsohler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "ServerConfig.hpp"
#include "Socket.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>


struct HTTPRequest {
	std::string method;
	std::string uri;
	std::string version;
	std::map<std::string, std::string> headers;
	std::string body;
};

std::vector<std::string> splitLines(const std::string& input) {
	std::vector<std::string> lines;
	std::istringstream stream(input);
	std::string line;
	while (getline(stream, line)) {
		std::cout << "Raw line: " << line << std::endl;
		lines.push_back(line);
	}
	return lines;
}

HTTPRequest parseHTTPRequest(const std::string& request) {
	HTTPRequest httpRequest;
	std::vector<std::string> lines = splitLines(request);

	std::istringstream firstLineStream(lines[0]);
	firstLineStream >> httpRequest.method >> httpRequest.uri >> httpRequest.version;

	for (size_t i = 1; i < lines.size(); ++i) {
		size_t colonPos = lines[i].find(':');
		if (colonPos != std::string::npos) {
			std::string headerName = lines[i].substr(0, colonPos);
			std::string headerValue = lines[i].substr(colonPos + 2);
			httpRequest.headers.insert(make_pair(headerName, headerValue));
		}
	}

	size_t emptyLineIndex = request.find("\r\n\r\n");
	if (emptyLineIndex != std::string::npos) {
		httpRequest.body = request.substr(emptyLineIndex + 4);
	}

	return httpRequest;
}

void printHTTPRequest(const HTTPRequest& httpRequest) {
	std::cout<< "Method: " << httpRequest.method << std::endl;
	std::cout<< "URI: " << httpRequest.uri << std::endl;
	std::cout<< "HTTP Version: " << httpRequest.version << std::endl;
	std::cout<< "Headers:" << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = httpRequest.headers.begin(); it != httpRequest.headers.end(); ++it) {
		std::cout<< it->first << ": " << it->second << std::endl;
	}
	std::cout<< "Body: " << httpRequest.body << std::endl;
}

struct URI {
	std::string scheme;
	std::string authority;
	std::string path;
	std::string query;
	std::string fragment;
};

URI parseURI(const std::string &uriString) {
	URI uri;

	size_t schemeEnd = uriString.find("://");
	if (schemeEnd != std::string::npos) {
		uri.scheme = uriString.substr(0, schemeEnd);
	}

	size_t authorityStart = schemeEnd != std::string::npos ? schemeEnd + 3 : 0;
	size_t pathStart = uriString.find("/", authorityStart);
	if (pathStart != std::string::npos) {
		uri.authority = uriString.substr(authorityStart, pathStart - authorityStart);
	}
	else {
		uri.authority = uriString.substr(authorityStart);
		return uri;
	}

	size_t queryStart = uriString.find("?", pathStart);
	size_t fragmentStart = uriString.find("#", pathStart);
	if (queryStart != std::string::npos) {
		uri.path = uriString.substr(pathStart, queryStart - pathStart);
	}
	else if (fragmentStart != std::string::npos) {
		uri.path = uriString.substr(pathStart, fragmentStart - pathStart);
	}
	else {
		uri.path = uriString.substr(pathStart);
	}

	if (queryStart != std::string::npos) {
		if (fragmentStart != std::string::npos) {
			uri.query = uriString.substr(queryStart + 1, fragmentStart - queryStart - 1);
		} else {
			uri.query = uriString.substr(queryStart + 1);
		}
	}

	if (fragmentStart != std::string::npos) {
		uri.fragment = uriString.substr(fragmentStart + 1);
	}

	return uri;
}

int main() {
	std::string httpRequestString = "GET /index.html?q=term#section2 HTTP/1.1\r\n"
								"Host: example.com\r\n"
								"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/96.0.4664.110 Safari/537.36\r\n"
								"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\n"
								"\r\n"
								"This is the request body.\nlol\n"
								"This is the request body.\nlol\n"
								"This is the request body.\nlol\n"
								"This is the request body.\nlol\n";

	HTTPRequest parsedRequest = parseHTTPRequest(httpRequestString);

	printHTTPRequest(parsedRequest);

	URI uri = parseURI(parsedRequest.uri);

	std::cout<< "Scheme: " << uri.scheme << std::endl;
	std::cout<< "Authority: " << uri.authority << std::endl;
	std::cout<< "Path: " << uri.path << std::endl;
	std::cout<< "Query: " << uri.query << std::endl;
	std::cout<< "Fragment: " << uri.fragment << std::endl;
	return 0;
}

/*
content lenght transfere encoding ensemble
*/

/*
Voici une liste des en-têtes HTTP couramment utilisés dans les requêtes HTTP :

1. `Accept`: Spécifie les types de contenu acceptables pour la réponse. OUI
2. `Accept-Encoding`: Indique les encodages de contenu acceptables pour la réponse. OUI
3. `Accept-Language`: Spécifie les langues préférées pour la réponse. OUI
4. `Authorization`: Contient les informations d'identification requises pour accéder à la ressource. OUI
5. `Cache-Control`: Spécifie les directives de contrôle du cache pour les caches le long de la chaîne de requête/réponse. OSEF
6. `Connection`: Contrôle les options de connexion pour la requête. OUI
7. `Content-Length`: Indique la taille du corps de la requête en octets. OUI
8. `Content-Type`: Indique le type de contenu du corps de la requête. OUI
9. `Cookie`: Contient des informations d'état de session. OUI
10. `Host`: Spécifie l'hôte et le numéro de port du serveur. OUI
11. `Origin`: Indique l'origine de la requête cross-origin. OUI
12. `Referer` (ou `Referer`): Indique l'URL de la ressource provenant de laquelle la requête actuelle a été initiée. OUI
13. `User-Agent`: Contient des informations sur l'agent utilisateur faisant la requête. PEUT ETRE BYE BYE

Notez que la présence et la signification de certains en-têtes peuvent
varier en fonction du contexte et des exigences spécifiques de l'application.
Vous pouvez ajouter ou supprimer des en-têtes en fonction de vos besoins particuliers.
*/

/*
La RFC HTTP 1.1 définit un ensemble d'en-têtes HTTP standardisés. Voici une liste exhaustive de ces en-têtes :

1. `Accept`
2. `Accept-Charset`
3. `Accept-Encoding`
4. `Accept-Language`
5. `Accept-Ranges`
6. `Age`
7. `Allow`
8. `Authorization`
9. `Cache-Control`
10. `Connection`
11. `Content-Encoding`
12. `Content-Language`
13. `Content-Length`
14. `Content-Location`
15. `Content-MD5`
16. `Content-Range`
17. `Content-Type`
18. `Date`
19. `ETag`
20. `Expect`
21. `Expires`
22. `From`
23. `Host`
24. `If-Match`
25. `If-Modified-Since`
26. `If-None-Match`
27. `If-Range`
28. `If-Unmodified-Since`
29. `Last-Modified`
30. `Location`
31. `Max-Forwards`
32. `Pragma`
33. `Proxy-Authenticate`
34. `Proxy-Authorization`
35. `Range`
36. `Referer`
37. `Retry-After`
38. `Server`
39. `TE`
40. `Trailer`
41. `Transfer-Encoding`
42. `Upgrade`
43. `User-Agent`
44. `Vary`
45. `Via`
46. `Warning`
47. `WWW-Authenticate`
48. `X-Forwarded-For`

Ces en-têtes sont définis dans la RFC HTTP 1.1 et sont largement
utilisés dans les applications web conformes à cette spécification.
Ils fournissent un moyen standardisé de communiquer des informations entre les clients et les serveurs HTTP.
*/
