#include "Peer.hpp"

#include <arpa/inet.h>
#include <unistd.h>

#include <cstring>

Peer::Peer() {
	this->_status = EMPTY;
	this->_sockfd = 0;
	std::memset(&this->_addr, 0, sizeof(this->_addr));
	this->_request = NULL;
	this->_lastActivity = 0;
}
Peer::~Peer() {
	if (this->_request) {
		delete this->_request;
	}
}

void Peer::connect(int sockfd, struct sockaddr_in addr, Server* server) {
	if (this->_status == EMPTY) {
		this->_sockfd = sockfd;
		this->_addr = addr;
		this->_status = CONNECTED;
		this->_server = server;
	}
}
void Peer::setRequest(std::string const &buffer) {
	this->_request = new Request(buffer);
	this->_status = WAITING_READ;
}
void Peer::setReponse(std::string const &response) {
	this->_response = response.c_str();
	this->_status = WAITING_WRITE;
}
void Peer::setLastActivity() {
	this->_lastActivity = time(NULL);
}
void Peer::reset() {
	close(this->_sockfd);
	this->_sockfd = 0;
	this->_status = EMPTY;
	std::memset(&this->_addr, 0, sizeof(this->_addr));
	delete this->_request;
	this->_request = NULL;
	this->_response.clear();
	this->_lastActivity = 0;
}

std::string Peer::findErrorPage(unsigned int errorCode) const {
	std::map<int, std::string>					error_page = _server->getConfig().getErrorPage();
	std::map<int, std::string>::const_iterator	it = error_page.find(errorCode);

	if (it != error_page.end()) {
		return _server->getConfig().getRoot() + it->second;
	} else {
		return DEFAULT_ERROR_PAGE;
	}
}

bool canOpenFile(const std::string& filename) {
	std::ifstream file(filename);
	bool isOpen = file.is_open();
	file.close();
	return isOpen;
}
// treat request a reecrire:
// fonction qui parse la request et qui renvoie un code de retour
// si le code n'est pas 200 find error page
// findErrorPage devrait aussi changer la valeur du pointeur filename pour une page d'erreur
unsigned int	Peer::treatRequest(std::string* filename) {
	std::string							requestedFile = _request->getURI().path;
	ServerConfig						config = _server->getConfig();
	const std::map<std::string, Route>&	routes = config.getRoutes();
	Route								routeFound;
	unsigned int						requestCode = 200;
	std::map<std::string, Route>::const_iterator it = routes.find(requestedFile);
	if (it != routes.end()) {
		routeFound = it->second;
	} else {
		requestCode = 404;
		*filename = this->findErrorPage(requestCode);
		return requestCode;
	}
	bool methodAllowed = false;
	for (size_t i = 0; i < routeFound.methods.size(); ++i) {
		if (routeFound.methods[i] == _request->getMethod()) {
			methodAllowed = true;
			break;
		}
	}
	if (!methodAllowed) {
		*filename = this->findErrorPage(requestCode);
		requestCode = 405;
		return requestCode;
	}
	// *filename = config.getRoot() + routeFound.root + routeFound.location + requestedFile.substr(routeFound.location.size());
	*filename = config.getRoot() + routeFound.root + requestedFile;
	if (canOpenFile(*filename) == false) {
		requestCode = 500;
		*filename = CRITICAL_ERROR_PAGE;
	}
	return requestCode;
}

std::string		Peer::generateResponseHeader(unsigned int requestCode) {
	std::string header;

	switch (requestCode) {
		case 200:
			header = "HTTP/1.1 200 OK\r\n";
			break;
		case 400:
			header = "HTTP/1.1 400 Bad Request\r\n";
			break;
		case 401:
			header = "HTTP/1.1 401 Unauthorized\r\n";
			break;
		case 403:
			header = "HTTP/1.1 403 Forbidden\r\n";
			break;
		case 404:
			header = "HTTP/1.1 404 Not Found\r\n";
			break;
		case 405:
			header = "HTTP/1.1 405 Method Not Allowed\r\n";
			break;
		case 500:
			header = "HTTP/1.1 500 Internal Server Error\r\n";
			break;
		case 501:
			header = "HTTP/1.1 501 Not Implemented\r\n";
			break;
		default:
			header = "HTTP/1.1 500 Internal Server Error\r\n";  // Par défaut, retourne une erreur interne du serveur
			break;
	}
	return header;
}

bool isContentTypeHtml(const std::string& content) {
	return (content.find("<html") != std::string::npos || 
			content.find("<!DOCTYPE html>") != std::string::npos);
}

std::string getContentType(const std::string& filename) {
	if (filename.find(".html") != std::string::npos || 
		filename.find(".htm") != std::string::npos) {
		return "text/html";
	}
	return "text/plain";
}

std::string Peer::generateResponseBody(const std::string& filename) {
	std::string body;
	std::string contentType = getContentType(filename);
	std::cout << "Filename: " << filename << std::endl;
	std::ifstream file(filename.c_str());
	if (file.is_open()) {
		std::stringstream buffer;
		buffer << file.rdbuf();
		std::string fileContent = buffer.str();

		if (isContentTypeHtml(fileContent)) {
			body = fileContent;
		} else {
			body = "Error: File is not of type text/html\r\n";
		}

		file.close();
	} else {
		body = "Error: Unable to open file\r\n";
	}

	std::string header = "";
	header += "Content-Type: " + contentType + "\r\n";
	header += "Content-Length: " + std::to_string(body.size()) + "\r\n";
	header += "Server: " + _server->getConfig().getServerName() + "\r\n";
	header += "\r\n";

	return header + body;
}
