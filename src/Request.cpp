/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsohler <lsohler@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/06 18:51:50 by lsohler           #+#    #+#             */
/*   Updated: 2024/05/09 18:05:51 by lsohler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

/*
Berners-Lee, et al.         Standards Track                    [Page 12]
RFC 3986                   URI Generic Syntax               January 2005


      reserved    = gen-delims / sub-delims

      gen-delims  = ":" / "/" / "?" / "#" / "[" / "]" / "@"

      sub-delims  = "!" / "$" / "&" / "'" / "(" / ")"
                  / "*" / "+" / "," / ";" / "="


2.3.  Unreserved Characters

   Characters that are allowed in a URI but do not have a reserved
   purpose are called unreserved.  These include uppercase and lowercase
   letters, decimal digits, hyphen, period, underscore, and tilde.

      unreserved  = ALPHA / DIGIT / "-" / "." / "_" / "~"


3.  Syntax Components

   The generic URI syntax consists of a hierarchical sequence of
   components referred to as the scheme, authority, path, query, and
   fragment.

      URI         = scheme ":" hier-part [ "?" query ] [ "#" fragment ]

      hier-part   = "//" authority path-abempty
                  / path-absolute
                  / path-rootless
                  / path-empty

   The scheme and path components are required, though the path may be
   empty (no characters).  When authority is present, the path must
   either be empty or begin with a slash ("/") character.  When
   authority is not present, the path cannot begin with two slash
   characters ("//").  These restrictions result in five different ABNF
   rules for a path (Section 3.3), only one of which will match any
   given URI reference.

   The following are two example URIs and their component parts:

         foo://example.com:8042/over/there?name=ferret#nose
         \_/   \______________/\_________/ \_________/ \__/
          |           |            |            |        |
       scheme     authority       path        query   fragment
*/

typedef std::map<int, std::string> ErrorMap;

void fillErrorMap(ErrorMap& errorMap) {
	errorMap[400] = "Bad Request";
	errorMap[401] = "Unauthorized";
	errorMap[402] = "Payment Required";
	errorMap[403] = "Forbidden";
	errorMap[404] = "Not Found";
	errorMap[405] = "Method Not Allowed";
	errorMap[406] = "Not Acceptable";
	errorMap[407] = "Proxy Authentication Required";
	errorMap[408] = "Request Timeout";
	errorMap[409] = "Conflict";
	errorMap[410] = "Gone";
	errorMap[411] = "Length Required";
	errorMap[412] = "Precondition Failed";
	errorMap[413] = "Payload Too Large";
	errorMap[414] = "URI Too Long";
	errorMap[415] = "Unsupported Media Type";
	errorMap[416] = "Range Not Satisfiable";
	errorMap[417] = "Expectation Failed";
	errorMap[418] = "I'm a teapot";
	errorMap[421] = "Misdirected Request";
	errorMap[422] = "Unprocessable Entity";
	errorMap[423] = "Locked";
	errorMap[424] = "Failed Dependency";
	errorMap[426] = "Upgrade Required";
	errorMap[428] = "Precondition Required";
	errorMap[429] = "Too Many Requests";
	errorMap[431] = "Request Header Fields Too Large";
	errorMap[451] = "Unavailable For Legal Reasons";
}

static const std::string header_fields[] = {
		"Accept",
		"Accept-Charset",
		"Accept-Encoding",
		"Accept-Language",
		"Accept-Ranges",
		"Age",
		"Allow",
		"Authorization",
		"Cache-Control",
		"Connection",
		"Content-Encoding",
		"Content-Language",
		"Content-Length",
		"Content-Location",
		"Content-MD5",
		"Content-Range",
		"Content-Type",
		"Date",
		"ETag",
		"Expect",
		"Expires",
		"From",
		"Host",
		"If-Match",
		"If-Modified-Since",
		"If-None-Match",
		"If-Range",
		"If-Unmodified-Since",
		"Last-Modified",
		"Location",
		"Max-Forwards",
		"Pragma",
		"Proxy-Authenticate",
		"Proxy-Authorization",
		"Range",
		"Referer",
		"Retry-After",
		"Server",
		"TE",
		"Trailer",
		"Transfer-Encoding",
		"Upgrade",
		"User-Agent",
		"Vary",
		"Via",
		"Warning",
		"WWW-Authenticate",
		"X-Forwarded-For"
};

static const std::string methods_syntax[] = {"GET", "POST", "DELETE"};

static const std::string gen_delims = ":/?#[]@";

static const std::string sub_delims = "!$&'()*+,;=";

static const std::string unreserved[] = {"ABCDEFGHIJKLMNOPQRSTUVWXYZ", "abcdefghijklmnopqrstuvwxyz", "0123456789", "-._~"};

bool is_gen_delim(char character) {
	return gen_delims.find(character) != std::string::npos;
}

bool is_sub_delim(char character) {
	return sub_delims.find(character) != std::string::npos;
}

bool is_unreserved(char character) {
	for (size_t i = 0; i < sizeof(unreserved) / sizeof(unreserved[0]); ++i) {
		if (unreserved[i].find(character) != std::string::npos) {
			return true;
		}
	}
	return false;
}

/*
Avec l'exemple /path/to/resource/cgi.php/additional_info?param1=value1&param2=value2
je veux dans path: /path/to/resource/cgi.php
pathInfo: /additional_info
querry: param1=value1&param2=value2
*/
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
	// size_t schemeEnd = uriString.find("://");
	// if (schemeEnd != std::string::npos) {
	// 	uri.scheme = uriString.substr(0, schemeEnd);
	// }

	// size_t authorityStart = schemeEnd != std::string::npos ? schemeEnd + 3 : 0;
	// size_t pathStart = uriString.find("/", authorityStart);
	// if (pathStart != std::string::npos) {
	// 	uri.authority = uriString.substr(authorityStart, pathStart - authorityStart);
	// } else {
	// 	uri.authority = uriString.substr(authorityStart);
	// 	return uri;
	// }

	// size_t queryStart = uriString.find("?", pathStart);
	// size_t fragmentStart = uriString.find("#", pathStart);
	// if (queryStart != std::string::npos) {
	// 	uri.path = uriString.substr(pathStart, queryStart - pathStart);
	// } else if (fragmentStart != std::string::npos) {
	// 	uri.path = uriString.substr(pathStart, fragmentStart - pathStart);
	// } else {
	// 	uri.path = uriString.substr(pathStart);
	// }

	// if (queryStart != std::string::npos) {
	// 	if (fragmentStart != std::string::npos) {
	// 		uri.query = uriString.substr(queryStart + 1, fragmentStart - queryStart - 1);
	// 	} else {
	// 		uri.query = uriString.substr(queryStart + 1);
	// 	}
	// }

	// return uri;
// }

std::vector<std::string> splitLines(const std::string& input) {
	std::vector<std::string> lines;
	std::istringstream stream(input);
	std::string line;
	while (getline(stream, line)) {
		// std::cout << "Raw line: " << line << std::endl;
		lines.push_back(line);
	}
	return lines;
}

std::string URIDecoder(const std::string &encoded) {
    std::stringstream decoded;
    std::string::const_iterator it = encoded.begin();
    while (it != encoded.end()) {
        if (*it == '%') {
            ++it;
            char hex1 = *it++;
            char hex2 = *it++;
            int hexValue;
            std::stringstream hexStream;
            hexStream << hex1 << hex2;				// colle les deux exas, et le transforme en
            hexStream >> std::hex >> hexValue;		//		un int qui est ensuite casté en char
            decoded << static_cast<char>(hexValue);	//		pour aller dans un stringstream
        } else {
            decoded << *it++;
        }
    }
    return decoded.str();
}

HTTPRequest parseHTTPRequest(const std::string& request) {
	HTTPRequest httpRequest;
	std::vector<std::string> lines = splitLines(request);

	std::istringstream firstLineStream(lines[0]);
	firstLineStream >> httpRequest.method >> httpRequest.uri >> httpRequest.version;
	httpRequest.uri = URIDecoder(httpRequest.uri); // decode UTF-8 en ascii

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

bool	isValidRequest(HTTPRequest httpRequest) {
	(void)httpRequest;
	return true;
}

std::string Request::toString() const {
	std::ostringstream oss;
	oss << this->_method << " " << this->_rawURI << " " << this->_version << "\r\n";
	std::map<std::string, std::string>::const_iterator it = _headers.begin();
	for (; it != this->_headers.end(); it++) {
		oss << it->first << ": " << it->second;
	}
	oss << "\r\n";
	return oss.str();
}

Request::Request(const std::string &httpRequestString) : _method(), _version(), _rawURI(), _URI(), _headers() {
	HTTPRequest	httpRequest = parseHTTPRequest(httpRequestString);
	if (isValidRequest(httpRequest)) {
		_method = httpRequest.method;
		_version = httpRequest.version;
		_rawURI = httpRequest.uri;
		_URI = parseURI(httpRequest.uri);
		_headers = httpRequest.headers;
		_body = httpRequest.body;
	}
}

void Request::printRequest() const {
	std::cout << "Method: " << _method << std::endl;
	std::cout << "Version: " << _version << std::endl;
	std::cout << "Raw URI: " << _rawURI << std::endl;
	// std::cout << "URI Scheme: " << _URI.scheme << std::endl;
	// std::cout << "URI Authority: " << _URI.authority << std::endl;
	// std::cout << "URI Path: " << _URI.path << std::endl;
	// std::cout << "URI Query: " << _URI.query << std::endl;
	// std::cout << "URI Fragment: " << _URI.fragment << std::endl;
	std::cout << "URI Path: " << _URI.path << std::endl;
	std::cout << "URI PathInfo: " << _URI.pathInfo << std::endl;
	std::cout << "URI Querry: " << _URI.querryString << std::endl;
	std::cout << "Headers:" << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it) {
		std::cout << "    " << it->first << ": " << it->second << std::endl;
	}
	std::cout << "Body: " << _body << std::endl;
}
