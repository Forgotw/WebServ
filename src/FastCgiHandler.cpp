/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FastCgiHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsohler <lsohler@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/01 13:46:37 by lsohler           #+#    #+#             */
/*   Updated: 2024/06/03 14:29:27 by lsohler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FastCgiHandler.hpp"
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <cstring>
#include <algorithm>
#include <arpa/inet.h>
// Constantes et structures FastCGI
#define FCGI_VERSION_1 1
#define FCGI_BEGIN_REQUEST 1
#define FCGI_ABORT_REQUEST 2
#define FCGI_END_REQUEST 3
#define FCGI_PARAMS 4
#define FCGI_STDIN 5
#define FCGI_STDOUT 6
#define FCGI_STDERR 7
#define FCGI_RESPONDER 1

struct FCGI_Header {
    uint8_t version;
    uint8_t type;
    uint16_t requestId;
    uint16_t contentLength;
    uint8_t paddingLength;
    uint8_t reserved;
};

struct FCGI_BeginRequestBody {
    unsigned char roleB1;
    unsigned char roleB0;
    unsigned char flags;
    unsigned char reserved[5];
};

struct FCGI_EndRequestBody {
    uint32_t appStatus;
    uint8_t protocolStatus;
    uint8_t reserved[3];
};

std::string uint16ToString(uint16_t value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}

std::string uint8ToString(uint8_t value) {
    std::stringstream ss;
    ss << static_cast<unsigned>(value);
    return ss.str();
}

// Fonction pour interpréter et afficher la requête FastCGI
std::string interpretFastCGIRequest(const std::string& fcgiRequest) {
    std::stringstream output;
    size_t pos = 0;

    while (pos < fcgiRequest.size()) {
        if (pos + sizeof(FCGI_Header) > fcgiRequest.size()) {
            output << "Incomplete FCGI_Header, remaining size: " << (fcgiRequest.size() - pos) << std::endl;
            break;
        }

        FCGI_Header header;
        std::memcpy(&header, &fcgiRequest[pos], sizeof(FCGI_Header));
        pos += sizeof(FCGI_Header);

        // Convertir en big-endian manuellement
        header.contentLength = (header.contentLength << 8) | (header.contentLength >> 8);
        header.requestId = (header.requestId << 8) | (header.requestId >> 8);

        output << "FCGI_Header:" << std::endl;
        output << "  version: " << uint8ToString(header.version) << std::endl;
        output << "  type: " << uint8ToString(header.type) << std::endl;
        output << "  requestId: " << uint16ToString(header.requestId) << std::endl;
        output << "  contentLength: " << uint16ToString(header.contentLength) << std::endl;
        output << "  paddingLength: " << uint8ToString(header.paddingLength) << std::endl;
        output << "  reserved: " << uint8ToString(header.reserved) << std::endl;

        if (pos + header.contentLength > fcgiRequest.size()) {
            output << "Incomplete content, remaining size: " << (fcgiRequest.size() - pos) << std::endl;
            break;
        }

        std::string content = fcgiRequest.substr(pos, header.contentLength);
        pos += header.contentLength + header.paddingLength;

        if (header.type == FCGI_BEGIN_REQUEST) {
            FCGI_BeginRequestBody body;
            std::memcpy(&body, content.data(), sizeof(FCGI_BeginRequestBody));

            // Combiner les octets pour obtenir le role
            uint16_t role = (body.roleB1 << 8) | body.roleB0;

            output << "FCGI_BeginRequestBody:" << std::endl;
            output << "  role: " << uint16ToString(role) << std::endl;
            output << "  flags: " << uint8ToString(body.flags) << std::endl;
        } else if (header.type == FCGI_PARAMS || header.type == FCGI_STDIN) {
            output << "Content: " << std::endl;
            for (size_t i = 0; i < content.size(); ++i) {
                output << content[i];
            }
            output << std::endl;
        } else {
            output << "Unknown content type" << std::endl;
        }
    }

    return output.str();
}


void printHex(const std::string& data) {
    for (std::size_t i = 0; i < data.size(); ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (static_cast<int>(data[i]) & 0xff) << " ";
        if ((i + 1) % 16 == 0) {
            std::cout << std::endl;
        }
    }
    std::cout << std::dec << std::endl; // Reset the stream to decimal format
}
// Fonction pour créer un en-tête FastCGI
std::vector<uint8_t> createHeader(uint8_t type, uint16_t requestId, uint16_t contentLength, uint8_t paddingLength = 0) {
    std::vector<uint8_t> header(8);
    header[0] = 1; // FCGI_VERSION_1
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
        std::cout << "createStdinRecord\n";
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

    // Ajouter l'enregistrement FCGI_BEGIN_REQUEST
    std::vector<uint8_t> beginRequestRecord = createBeginRequestRecord(requestId);
    request.insert(request.end(), beginRequestRecord.begin(), beginRequestRecord.end());

    // Ajouter les enregistrements FCGI_PARAMS
    std::vector<uint8_t> paramsRecord = createParamsRecord(requestId, envp);
    request.insert(request.end(), paramsRecord.begin(), paramsRecord.end());

    // Ajouter les enregistrements FCGI_STDIN
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
        std::cout << "receiveFastCGIResponse\n";
        response.append(buffer, bytesRead);
    }
    if (bytesRead < 0) {
        perror("recv");
        return "Status: 500\r\n\r\nFastCgi Php-Fpm receive error"; // TODO: remove
    }
    return response;
}

std::string interpretFastCGIResponse(const std::string& response) {
    size_t pos = 0;
    std::string stdoutContent;
    std::string stderrContent;
    while (pos < response.size()) {
        std::cout << "interpretFastCGIResponse\n";
        FCGI_Header header;
        std::memcpy(&header, &response[pos], sizeof(FCGI_Header));
        pos += sizeof(FCGI_Header);

        header.contentLength = (header.contentLength << 8) | (header.contentLength >> 8); // Convertir en big-endian
        header.requestId = (header.requestId << 8) | (header.requestId >> 8); // Convertir en big-endian

        std::string content = response.substr(pos, header.contentLength);
        pos += header.contentLength + header.paddingLength;

        if (header.type == FCGI_STDOUT) {
            stdoutContent += content;
        } else if (header.type == FCGI_STDERR) {
            stderrContent += content;
        } else if (header.type == FCGI_END_REQUEST) {
            FCGI_EndRequestBody endRequestBody;
            std::memcpy(&endRequestBody, content.data(), sizeof(endRequestBody));
            break; // Fin de la requête
        }
    }

    if (!stderrContent.empty()) {
        std::cerr << "Error: " << stderrContent << std::endl;
    }

    return stdoutContent;
}

std::string FastCgiHandler::generateFastCgiResponse(char** envp, const std::string& fastcgi_pass, const Request& request) {
    std::cout << "FastCgiHandler:: fastcgi_pass:" << fastcgi_pass << std::endl;
    std::cout << "FastCgiHandler:: getBody:" << request.getBody() << std::endl;
    int sock = createFastCGIConnection(fastcgi_pass);
    if (sock < 0) {
        // *uiStatusCode = 500;
        perror("sock");
        return "Status: 500\r\n\r\nFastCgi Php-Fpm sock error";
    }
    unsigned int id = generateUniqueId();
    std::string fcgiRequestBin = constructFastCGIRequest(id, envp, request.getBody());
    // fcgiRequestBin += request.getBody();
    std::cout << "-----------------------------------------\n";
    std::cout << "----" << fastcgi_pass<< "----\n";
    std::cout << "-----------------------------------------\n";
    // printHex(fcgiRequestBin);
    std::cout << interpretFastCGIRequest(fcgiRequestBin);
    std::cout << "-----------------------------------------\n";
    // std::cout << "-----------------------------------------\n";
    std::cout << "----" << "request end" << "----\n";
    std::cout << "-----------------------------------------\n";
    ssize_t bytesSent = write(sock, fcgiRequestBin.c_str(), fcgiRequestBin.size());
    if (bytesSent < 0) {
        perror("write");
        close(sock);
        return "Status: 500\r\n\r\nFastCgi Php-Fpm write error";
    }
    std::cout << "After fcgi write in socket\n";
    // Receive the response from PHP-FPM
    std::string fcgiResponseBin = receiveFastCGIResponse(sock);
    std::cout << "After fcgiResponseBin\n";
    close(sock);
    // std::cout << "-----------------------------------------\n";
    // std::cout << "----" << respFCGI << "----\n";
    // std::cout << "-----------------------------------------\n";
    // // printHex(respFCGI);
    // // std::cout << respFCGI;
    std::string response = interpretFastCGIResponse(fcgiResponseBin);
    std::cout << "After interpretFastCGIResponse\n";
    std::ofstream outFile("test", std::ios::binary);
    if (outFile.is_open()) {
        outFile.write(fcgiResponseBin.c_str(), fcgiResponseBin.size());
        outFile.close();
    } else {
        std::cerr << "Unable to open file for writing" << std::endl;
    }
    // std::cout << "-----------------------------------------\n";
    // std::cout << "-----------------------------------------\n";
    // std::cout << "-----------------------------------------\n";
    return response; // TODO: remove
}
