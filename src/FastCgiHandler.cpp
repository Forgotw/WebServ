/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FastcgiHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsohler <lsohler@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/01 13:46:37 by lsohler           #+#    #+#             */
/*   Updated: 2024/06/01 17:40:07 by lsohler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FastCgiHandler.hpp"
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <vector>
#include <cstring>
#include <algorithm>

// Constantes et structures FastCGI
#define FCGI_VERSION_1 1
#define FCGI_BEGIN_REQUEST 1
#define FCGI_PARAMS 4
#define FCGI_STDIN 5

#define FCGI_RESPONDER 1

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

struct FCGI_BeginRequestBody {
    unsigned char roleB1;
    unsigned char roleB0;
    unsigned char flags;
    unsigned char reserved[5];
};

std::vector<uint8_t> createHeader(uint8_t type, uint16_t requestId, uint16_t contentLength, uint8_t paddingLength = 0) {
    std::vector<uint8_t> header(8);
    header[0] = FCGI_VERSION_1;
    header[1] = type;
    header[2] = (requestId >> 8) & 0xff;
    header[3] = requestId & 0xff;
    header[4] = (contentLength >> 8) & 0xff;
    header[5] = contentLength & 0xff;
    header[6] = paddingLength;
    header[7] = 0;
    return header;
}

std::vector<uint8_t> createBeginRequestRecord(uint16_t requestId) {
    std::vector<uint8_t> record;
    std::vector<uint8_t> header = createHeader(FCGI_BEGIN_REQUEST, requestId, sizeof(FCGI_BeginRequestBody));
    record.insert(record.end(), header.begin(), header.end());

    FCGI_BeginRequestBody body;
    body.roleB1 = 0;
    body.roleB0 = FCGI_RESPONDER;
    body.flags = 0;
    std::memset(body.reserved, 0, sizeof(body.reserved));

    record.insert(record.end(), reinterpret_cast<uint8_t*>(&body), reinterpret_cast<uint8_t*>(&body) + sizeof(body));
    return record;
}

std::vector<uint8_t> createNameValuePair(const std::string& name, const std::string& value) {
    std::vector<uint8_t> result;

    uint32_t nameLength = name.size();
    uint32_t valueLength = value.size();

    if (nameLength < 128) {
        result.push_back(static_cast<uint8_t>(nameLength));
    } else {
        result.push_back(static_cast<uint8_t>((nameLength >> 24) | 0x80));
        result.push_back(static_cast<uint8_t>(nameLength >> 16));
        result.push_back(static_cast<uint8_t>(nameLength >> 8));
        result.push_back(static_cast<uint8_t>(nameLength));
    }

    if (valueLength < 128) {
        result.push_back(static_cast<uint8_t>(valueLength));
    } else {
        result.push_back(static_cast<uint8_t>((valueLength >> 24) | 0x80));
        result.push_back(static_cast<uint8_t>(valueLength >> 16));
        result.push_back(static_cast<uint8_t>(valueLength >> 8));
        result.push_back(static_cast<uint8_t>(valueLength));
    }

    result.insert(result.end(), name.begin(), name.end());
    result.insert(result.end(), value.begin(), value.end());

    return result;
}

std::vector<uint8_t> createParamsRecord(uint16_t requestId, char** envp) {
    std::vector<uint8_t> record;
    for (int i = 0; envp[i] != NULL; ++i) {
        std::string env_entry(envp[i]);
        std::size_t pos = env_entry.find('=');
        if (pos != std::string::npos) {
            std::string key = env_entry.substr(0, pos);
            std::string value = env_entry.substr(pos + 1);
            std::vector<uint8_t> nameValuePair = createNameValuePair(key, value);
            std::vector<uint8_t> header = createHeader(FCGI_PARAMS, requestId, nameValuePair.size());
            record.insert(record.end(), header.begin(), header.end());
            record.insert(record.end(), nameValuePair.begin(), nameValuePair.end());
        }
    }
    record.insert(record.end(), createHeader(FCGI_PARAMS, requestId, 0).begin(), createHeader(FCGI_PARAMS, requestId, 0).end());
    return record;
}

std::vector<uint8_t> createStdinRecord(uint16_t requestId, const std::string& requestBody) {
    std::vector<uint8_t> record;
    size_t offset = 0;
    while (offset < requestBody.size()) {
        size_t chunkSize = std::min(requestBody.size() - offset, static_cast<size_t>(65535));
        std::vector<uint8_t> header = createHeader(FCGI_STDIN, requestId, chunkSize);
        record.insert(record.end(), header.begin(), header.end());
        record.insert(record.end(), requestBody.begin() + offset, requestBody.begin() + offset + chunkSize);
        offset += chunkSize;
    }
    record.insert(record.end(), createHeader(FCGI_STDIN, requestId, 0).begin(), createHeader(FCGI_STDIN, requestId, 0).end());
    return record;
}

std::string constructFastCGIRequest(uint16_t requestId, char** envp, const std::string& requestBody) {
    std::vector<uint8_t> request;

    std::vector<uint8_t> beginRequestRecord = createBeginRequestRecord(requestId);
    request.insert(request.end(), beginRequestRecord.begin(), beginRequestRecord.end());

    std::vector<uint8_t> paramsRecord = createParamsRecord(requestId, envp);
    request.insert(request.end(), paramsRecord.begin(), paramsRecord.end());

    std::vector<uint8_t> stdinRecord = createStdinRecord(requestId, requestBody);
    request.insert(request.end(), stdinRecord.begin(), stdinRecord.end());

    return std::string(request.begin(), request.end());
}

std::string FastCgiHandler::setFastCgiPass(const Location* foundLocation, const Location* cgiLocation) {
    if (!foundLocation->getFastCgiPass().empty()) {
        return foundLocation->getFastCgiPass();
    }
    if (!cgiLocation->getFastCgiPass().empty()) {
        return cgiLocation->getFastCgiPass();
    }
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

int createFastCGIConnection(const std::string& fastcgi_pass) {
    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) {
        // throw std::runtime_error(std::string("socket: ") + std::strerror(errno));
        return -1;
    }

    struct sockaddr_un server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, fastcgi_pass.c_str(), sizeof(server_addr.sun_path) - 1);

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        close(sock);
        // throw std::runtime_error(std::string("connest: ") + std::strerror(errno));
        return -1;
    }

    return sock;
}

unsigned int generateUniqueId() {
    static unsigned int id = 0;
    return id++;
}

std::string receiveFastCGIResponse(int sock) {
    char buffer[8192];
    std::string response;
    ssize_t bytesRead;
    while ((bytesRead = recv(sock, buffer, sizeof(buffer), 0)) > 0) {
        response.append(buffer, bytesRead);
    }
    if (bytesRead < 0) {
        perror("recv");
        return "Status: 500\r\n\r\nFastCgi Php-Fpm receive error";
    }
    return response;
}

std::string FastCgiHandler::generateFastCgiResponse(char** envp, const std::string& fastcgi_pass, const Request& request) {
    int sock = createFastCGIConnection(fastcgi_pass);
    if (sock < 0) {
        // *uiStatusCode = 500;
        perror("sock");
        return "Status: 500\r\n\r\nFastCgi Php-Fpm sock error";
    }
    unsigned int id = generateUniqueId();
    std::string fcgiRequest = constructFastCGIRequest(id, envp, request.getBody());
    ssize_t bytesSent = write(sock, fcgiRequest.c_str(), fcgiRequest.size());
    if (bytesSent < 0) {
        perror("write");
        close(sock);
        return "Status: 500\r\n\r\nFastCgi Php-Fpm write error";
    }

    // Receive the response from PHP-FPM
    std::string respCGI = receiveFastCGIResponse(sock);
    close(sock);
}