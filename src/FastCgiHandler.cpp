/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FastCgiHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsohler <lsohler@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/01 13:46:37 by lsohler           #+#    #+#             */
/*   Updated: 2024/06/08 18:36:47 by lsohler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FastCgiHandler.hpp"
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <map>
#include <cstring>
#include <algorithm>
#include <sstream>

// Constants for FastCGI
#define FCGI_VERSION_1 1

#define FCGI_BEGIN_REQUEST 1
#define FCGI_ABORT_REQUEST 2
#define FCGI_END_REQUEST 3
#define FCGI_PARAMS 4
#define FCGI_STDIN 5
#define FCGI_STDOUT 6
#define FCGI_STDERR 7
#define FCGI_DATA 8
#define FCGI_GET_VALUES 9
#define FCGI_GET_VALUES_RESULT 10
#define FCGI_UNKNOWN_TYPE 11
#define FCGI_MAXTYPE FCGI_UNKNOWN_TYPE

#define FCGI_RESPONDER 1

#define FCGI_KEEP_CONN 1

// Structure of a FastCGI Header
struct FCGI_Header {
    unsigned char version;
    unsigned char type;
    unsigned char requestIdB1;
    unsigned char requestIdB0;
    unsigned char contentLengthB1;
    unsigned char contentLengthB0;
    unsigned char paddingLength;
    unsigned char reserved;
};

// Structure of a FastCGI Begin Request Body
struct FCGI_BeginRequestBody {
    unsigned char roleB1;
    unsigned char roleB0;
    unsigned char flags;
    unsigned char reserved[5];
};

// Function to create a FastCGI header
void createHeader(FCGI_Header &header, unsigned char type, unsigned short requestId, unsigned short contentLength, unsigned char paddingLength) {
    header.version = FCGI_VERSION_1;
    header.type = type;
    header.requestIdB1 = (requestId >> 8) & 0xff;
    header.requestIdB0 = requestId & 0xff;
    header.contentLengthB1 = (contentLength >> 8) & 0xff;
    header.contentLengthB0 = contentLength & 0xff;
    header.paddingLength = paddingLength;
    header.reserved = 0;
}

// Function to create a FastCGI name-value pair
std::string createNameValuePair(const std::string &name, const std::string &value) {
    std::string pair;
    unsigned char nameLength = name.size();
    unsigned char valueLength = value.size();
    pair += nameLength;
    pair += valueLength;
    pair += name;
    pair += value;
    return pair;
}

// void interpretAndPrintFCGIRequest(const std::string& fcgiRequestBin) {
//     size_t pos = 0;

//     while (pos < fcgiRequestBin.size()) {
//         // Lire l'en-tête FastCGI
//         FCGI_Header header;
//         std::memcpy(&header, fcgiRequestBin.data() + pos, sizeof(header));
//         header.requestId = ntohs(header.requestId);
//         header.contentLength = ntohs(header.contentLength);
//         pos += sizeof(header);

//         // Afficher l'en-tête
//         std::cout << "FCGI Header:" << std::endl;
//         std::cout << "  Version: " << static_cast<int>(header.version) << std::endl;
//         std::cout << "  Type: " << static_cast<int>(header.type) << std::endl;
//         std::cout << "  Request ID: " << header.requestId << std::endl;
//         std::cout << "  Content Length: " << header.contentLength << std::endl;
//         std::cout << "  Padding Length: " << static_cast<int>(header.paddingLength) << std::endl;
//         std::cout << "  Reserved: " << static_cast<int>(header.reserved) << std::endl;

//         // Lire le contenu
//         std::string content(fcgiRequestBin.data() + pos, header.contentLength);
//         pos += header.contentLength + header.paddingLength;

//         // Afficher le contenu en fonction du type
//         switch (header.type) {
//             case FCGI_BEGIN_REQUEST:
//                 {
//                     FCGI_BeginRequestBody beginRequestBody;
//                     std::memcpy(&beginRequestBody, content.data(), sizeof(beginRequestBody));
//                     beginRequestBody.role = ntohs(beginRequestBody.role);

//                     std::cout << "FCGI Begin Request Body:" << std::endl;
//                     std::cout << "  Role: " << beginRequestBody.role << std::endl;
//                     std::cout << "  Flags: " << static_cast<int>(beginRequestBody.flags) << std::endl;
//                     std::cout << "  Reserved: ";
//                     for (size_t i = 0; i < sizeof(beginRequestBody.reserved); ++i) {
//                         std::cout << static_cast<int>(beginRequestBody.reserved[i]) << " ";
//                     }
//                     std::cout << std::endl;
//                 }
//                 break;
//             case FCGI_PARAMS:
//                 {
//                     std::cout << "FCGI Params:" << std::endl;
//                     size_t paramPos = 0;
//                     while (paramPos < content.size()) {
//                         uint32_t nameLength;
//                         uint32_t valueLength;

//                         if (content[paramPos] & 0x80) {
//                             nameLength = ((content[paramPos] & 0x7f) << 24) | (content[paramPos+1] << 16) | (content[paramPos+2] << 8) | content[paramPos+3];
//                             paramPos += 4;
//                         } else {
//                             nameLength = content[paramPos];
//                             paramPos += 1;
//                         }

//                         if (content[paramPos] & 0x80) {
//                             valueLength = ((content[paramPos] & 0x7f) << 24) | (content[paramPos+1] << 16) | (content[paramPos+2] << 8) | content[paramPos+3];
//                             paramPos += 4;
//                         } else {
//                             valueLength = content[paramPos];
//                             paramPos += 1;
//                         }

//                         std::string name = content.substr(paramPos, nameLength);
//                         paramPos += nameLength;
//                         std::string value = content.substr(paramPos, valueLength);
//                         paramPos += valueLength;

//                         std::cout << "  Name: " << name << ", Value: " << value << std::endl;
//                     }
//                 }
//                 break;
//             case FCGI_STDIN:
//                 std::cout << "FCGI Stdin: " << std::endl;
//                 std::cout << "  " << content << std::endl;
//                 break;
//             case FCGI_STDOUT:
//                 std::cout << "FCGI Stdout: " << std::endl;
//                 std::cout << "  " << content << std::endl;
//                 break;
//             case FCGI_STDERR:
//                 std::cerr << "FCGI Stderr: " << std::endl;
//                 std::cerr << "  " << content << std::endl;
//                 break;
//             case FCGI_END_REQUEST:
//                 std::cout << "FCGI End Request" << std::endl;
//                 break;
//             default:
//                 std::cout << "Unknown FCGI type: " << static_cast<int>(header.type) << std::endl;
//                 break;
//         }

//         std::cout << "-----------------------------------------" << std::endl;
//     }
// }

void printHex(const std::string& data) {
    for (std::size_t i = 0; i < data.size(); ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (static_cast<int>(data[i]) & 0xff) << " ";
        if ((i + 1) % 16 == 0) {
            std::cout << std::endl;
        }
    }
    std::cout << std::dec << std::endl; // Reset the stream to decimal format
}

std::string toString(uint16_t value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

std::string toString(uint8_t value) {
    std::ostringstream oss;
    oss << static_cast<int>(value);
    return oss.str();
}

std::string FastCgiHandler::setFastCgiPass(const Location* foundLocation, const Location* cgiLocation) {
    if (!foundLocation->getFastCgiPass().empty()) {
        return foundLocation->getFastCgiPass();
    }
    if (!cgiLocation->getFastCgiPass().empty()) {
        return cgiLocation->getFastCgiPass();
    }
    return "";
}

bool FastCgiHandler::isPhpExtension(const std::string& path) {
	size_t dotPosition = path.rfind('.');
	if (dotPosition != std::string::npos) {
		std::string extension = path.substr(dotPosition);
        if (extension == ".php") {
            return true;
        }
    }
    return false;
}


// std::string constructFastCGIRequest(int requestId, const std::map<std::string, std::string>& params, const std::string& body) {
//     FCGI_Header header;
//     FCGI_BeginRequestBody beginRequestBody;
//     std::string request;

//     // Header de la requête
//     header.version = FCGI_VERSION_1;
//     header.type = FCGI_BEGIN_REQUEST;
//     header.requestId = htons(requestId);
//     header.contentLength = htons(sizeof(FCGI_BeginRequestBody));
//     header.paddingLength = 0;
//     header.reserved = 0;

//     request.append(reinterpret_cast<char*>(&header), sizeof(header));

//     // Body de la requête
//     beginRequestBody.role = htons(FCGI_RESPONDER);
//     beginRequestBody.flags = 0;
//     memset(beginRequestBody.reserved, 0, sizeof(beginRequestBody.reserved));

//     request.append(reinterpret_cast<char*>(&beginRequestBody), sizeof(beginRequestBody));

//     // Ajout des paramètres
//     for (std::map<std::string, std::string>::const_iterator param = params.begin(); param != params.end(); param++) {
//         const std::string& name = param->first;
//         const std::string& value = param->second;
//         std::cout << param->first << param->second << std::endl;

//         uint32_t nameLength = name.size();
//         uint32_t valueLength = value.size();

//         if (nameLength < 128) {
//             request.push_back(static_cast<uint8_t>(nameLength));
//         } else {
//             request.push_back(static_cast<uint8_t>((nameLength >> 24) | 0x80));
//             request.push_back(static_cast<uint8_t>(nameLength >> 16));
//             request.push_back(static_cast<uint8_t>(nameLength >> 8));
//             request.push_back(static_cast<uint8_t>(nameLength));
//         }

//         if (valueLength < 128) {
//             request.push_back(static_cast<uint8_t>(valueLength));
//         } else {
//             request.push_back(static_cast<uint8_t>((valueLength >> 24) | 0x80));
//             request.push_back(static_cast<uint8_t>(valueLength >> 16));
//             request.push_back(static_cast<uint8_t>(valueLength >> 8));
//             request.push_back(static_cast<uint8_t>(valueLength));
//         }

//         request.append(name);
//         request.append(value);
//     }

//     // Ajouter un en-tête PARAMS vide pour indiquer la fin des paramètres
//     FCGI_Header paramsEndHeader;
//     paramsEndHeader.version = FCGI_VERSION_1;
//     paramsEndHeader.type = FCGI_PARAMS;
//     paramsEndHeader.requestId = htons(requestId);
//     paramsEndHeader.contentLength = 0;
//     paramsEndHeader.paddingLength = 0;
//     paramsEndHeader.reserved = 0;
//     request.append(reinterpret_cast<char*>(&paramsEndHeader), sizeof(paramsEndHeader));

//     // Ajout du corps de la requête
//     if (!body.empty()) {
//         FCGI_Header stdinHeader;
//         stdinHeader.version = FCGI_VERSION_1;
//         stdinHeader.type = FCGI_STDIN;
//         stdinHeader.requestId = htons(requestId);
//         stdinHeader.contentLength = htons(body.size());
//         stdinHeader.paddingLength = 0;
//         stdinHeader.reserved = 0;

//         request.append(reinterpret_cast<char*>(&stdinHeader), sizeof(stdinHeader));
//         request.append(body);
//     }

//     // Ajout de la fin de la requête STDIN
//     FCGI_Header stdinEndHeader;
//     stdinEndHeader.version = FCGI_VERSION_1;
//     stdinEndHeader.type = FCGI_STDIN;
//     stdinEndHeader.requestId = htons(requestId);
//     stdinEndHeader.contentLength = 0;
//     stdinEndHeader.paddingLength = 0;
//     stdinEndHeader.reserved = 0;

//     request.append(reinterpret_cast<char*>(&stdinEndHeader), sizeof(stdinEndHeader));

//     return request;
// }


// std::string receiveFastCGIResponse(int sock) {
//     std::string response;
//     char buffer[1024];
//     ssize_t bytesRead;

//     while ((bytesRead = read(sock, buffer, sizeof(buffer))) > 0) {
//         response.append(buffer, bytesRead);
//     }

//     return response;
// }

unsigned int generateUniqueId() {
    static unsigned int id = 0;
    return id++;
}

// std::string interpretFastCGIResponse(const std::string& response) {
//     std::string httpResponse;
//     size_t pos = 0;

//     while (pos < response.size()) {
//         // Lire l'en-tête FastCGI
//         FCGI_Header header;
//         std::memcpy(&header, response.data() + pos, sizeof(header));
//         header.requestId = ntohs(header.requestId);
//         header.contentLength = ntohs(header.contentLength);
//         pos += sizeof(header);

//         // Lire le contenu
//         std::string content(response.data() + pos, header.contentLength);
//         pos += header.contentLength + header.paddingLength;

//         if (header.type == FCGI_STDOUT) {
//             httpResponse += content;
//         } else if (header.type == FCGI_STDERR) {
//             std::cerr << "FastCGI Error: " << content << std::endl;
//         }
//     }

//     return httpResponse;
// }


int createFastCGIConnection(const std::string& fastcgi_pass) {
    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("inet_pton");
        // throw std::runtime_error(std::string("socket: ") + std::strerror(errno));
        return -1;
    }

    struct sockaddr_un server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, fastcgi_pass.c_str(), sizeof(server_addr.sun_path) - 1);

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(sock);
        // throw std::runtime_error(std::string("connest: ") + std::strerror(errno));
        return -1;
    }

    return sock;
}

std::string FastCgiHandler::handleFastCGIRequest(const std::string& fastcgi_pass, const std::map<std::string, std::string>& params, const std::string& stdinData) {
    int sock = createFastCGIConnection(fastcgi_pass);
    if (sock == -1) {
        return "Status: 500\r\n\r\nFastCgi Php-Fpm socket creation error";
    }

    // Begin Request
    FCGI_Header beginRequestHeader;
    FCGI_BeginRequestBody beginRequestBody;
    unsigned int id = generateUniqueId();
    createHeader(beginRequestHeader, FCGI_BEGIN_REQUEST, id, sizeof(beginRequestBody), 0);
    beginRequestBody.roleB1 = (FCGI_RESPONDER >> 8) & 0xff;
    beginRequestBody.roleB0 = FCGI_RESPONDER & 0xff;
    beginRequestBody.flags = 0;
    memset(beginRequestBody.reserved, 0, sizeof(beginRequestBody.reserved));

    send(sock, &beginRequestHeader, sizeof(beginRequestHeader), 0);
    send(sock, &beginRequestBody, sizeof(beginRequestBody), 0);

    // Params
    for (std::map<std::string, std::string>::const_iterator it = params.begin(); it != params.end(); ++it) {
        FCGI_Header paramHeader;
        std::string paramRecord = createNameValuePair(it->first, it->second);
        createHeader(paramHeader, FCGI_PARAMS, id, paramRecord.size(), 0);
        send(sock, &paramHeader, sizeof(paramHeader), 0);
        send(sock, paramRecord.c_str(), paramRecord.size(), 0);
    }
    // End of Params
    FCGI_Header endParamsHeader;
    createHeader(endParamsHeader, FCGI_PARAMS, id, 0, 0);
    send(sock, &endParamsHeader, sizeof(endParamsHeader), 0);

    // std::cout << "FCGI Body: " << stdinData << std::endl;
    // Stdin (for POST data)
    if (!stdinData.empty()) {
        FCGI_Header stdinHeader;
        createHeader(stdinHeader, FCGI_STDIN, id, stdinData.size(), 0);
        send(sock, &stdinHeader, sizeof(stdinHeader), 0);
        send(sock, stdinData.c_str(), stdinData.size(), 0);
    }

    // End of Stdin
    FCGI_Header endStdinHeader;
    createHeader(endStdinHeader, FCGI_STDIN, id, 0, 0);
    send(sock, &endStdinHeader, sizeof(endStdinHeader), 0);

    // Read the response
    std::string response;
    while (true) {
        FCGI_Header responseHeader;
        int bytesRead = recv(sock, &responseHeader, sizeof(responseHeader), 0);
        if (bytesRead <= 0) {
            break;
        }

        unsigned short contentLength = (responseHeader.contentLengthB1 << 8) + responseHeader.contentLengthB0;
        unsigned char paddingLength = responseHeader.paddingLength;

        if (contentLength > 0) {
            std::vector<char> contentData(contentLength);
            recv(sock, &contentData[0], contentLength, 0);
            if (responseHeader.type == FCGI_STDOUT) {
                response.append(contentData.begin(), contentData.end());
            }
        }

        if (paddingLength > 0) {
            std::vector<char> paddingData(paddingLength);
            recv(sock, &paddingData[0], paddingLength, 0);
        }

        if (responseHeader.type == FCGI_END_REQUEST) {
            break;
        }
    }

    close(sock);
    return response;
}
