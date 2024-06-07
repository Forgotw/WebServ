#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>

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

// Function to send a FastCGI request
std::string sendFastCGIRequest(const std::string &scriptFilename, const std::string &requestMethod, const std::string &queryString, const std::string &contentType, const std::string &stdinData) {
    // Create a socket to communicate with PHP-FPM
    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Failed to create socket" << std::endl;
        return "";
    }

    struct sockaddr_un serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sun_family = AF_UNIX;
    strcpy(serverAddr.sun_path, "/var/run/php/php7.4-fpm.sock"); // Modify to match your PHP-FPM socket

    if (connect(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Failed to connect to PHP-FPM" << std::endl;
        close(sock);
        return "";
    }

    // Begin Request
    FCGI_Header beginRequestHeader;
    FCGI_BeginRequestBody beginRequestBody;
    createHeader(beginRequestHeader, FCGI_BEGIN_REQUEST, 1, sizeof(beginRequestBody), 0);
    beginRequestBody.roleB1 = (FCGI_RESPONDER >> 8) & 0xff;
    beginRequestBody.roleB0 = FCGI_RESPONDER & 0xff;
    beginRequestBody.flags = 0;
    memset(beginRequestBody.reserved, 0, sizeof(beginRequestBody.reserved));

    send(sock, &beginRequestHeader, sizeof(beginRequestHeader), 0);
    send(sock, &beginRequestBody, sizeof(beginRequestBody), 0);

    // Params
    std::map<std::string, std::string> params;
    params["SCRIPT_FILENAME"] = scriptFilename;
    params["REQUEST_METHOD"] = requestMethod;
    params["QUERY_STRING"] = queryString;
    params["CONTENT_TYPE"] = contentType;
    params["CONTENT_LENGTH"] = std::to_string(stdinData.size());
    params["GATEWAY_INTERFACE"] = "CGI/1.1";
    params["SERVER_SOFTWARE"] = "MyServer/1.0";
    params["REMOTE_ADDR"] = "127.0.0.1";
    params["REMOTE_PORT"] = "12345";
    params["SERVER_ADDR"] = "127.0.0.1";
    params["SERVER_PORT"] = "80";
    params["SERVER_NAME"] = "localhost";
    params["SERVER_PROTOCOL"] = "HTTP/1.1";
    params["DOCUMENT_ROOT"] = "/var/www/html";
    params["SCRIPT_NAME"] = scriptFilename;
    params["REQUEST_URI"] = scriptFilename;
    params["HTTPS"] = "off";

    for (std::map<std::string, std::string>::iterator it = params.begin(); it != params.end(); ++it) {
        std::string paramRecord = createNameValuePair(it->first, it->second);
        FCGI_Header paramHeader;
        createHeader(paramHeader, FCGI_PARAMS, 1, paramRecord.size(), 0);
        send(sock, &paramHeader, sizeof(paramHeader), 0);
        send(sock, paramRecord.c_str(), paramRecord.size(), 0);
    }

    // End of Params
    FCGI_Header endParamsHeader;
    createHeader(endParamsHeader, FCGI_PARAMS, 1, 0, 0);
    send(sock, &endParamsHeader, sizeof(endParamsHeader), 0);

    // Stdin (for POST data)
    if (!stdinData.empty()) {
        FCGI_Header stdinHeader;
        createHeader(stdinHeader, FCGI_STDIN, 1, stdinData.size(), 0);
        send(sock, &stdinHeader, sizeof(stdinHeader), 0);
        send(sock, stdinData.c_str(), stdinData.size(), 0);
    }

    // End of Stdin
    FCGI_Header endStdinHeader;
    createHeader(endStdinHeader, FCGI_STDIN, 1, 0, 0);
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

int main() {
    std::string response = sendFastCGIRequest("/var/www/html/script.php", "POST", "", "application/x-www-form-urlencoded", "data=value");
    std::cout << response << std::endl;
    return 0;
}
