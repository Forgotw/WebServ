/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsohler <lsohler@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/11 15:54:52 by lsohler           #+#    #+#             */
/*   Updated: 2024/04/08 14:39:16 by lsohler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"
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

