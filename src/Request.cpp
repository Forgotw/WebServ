#include "Request.hpp"

#include <string>
#include <map>
#include <sstream>
#include <ostream>

Request::Request(std::string const &buffer) {
	parseBuffer(buffer);
}
Request::~Request() {}

std::string const &Request::getMethod() const { return this->_method; }
std::string const &Request::getUri() const { return this->_uri; }
std::string const &Request::getHttpVersion() const { return this->_httpVersion; }
std::map<std::string, std::string> const &Request::getHeaders() const { return this->_headers; }

void Request::parseBuffer(std::string const &buffer) {
	std::istringstream ss(buffer);
	std::string requestLine;
	if (std::getline(ss, requestLine)) {
		std::istringstream requestLineStream(requestLine);
		requestLineStream >> this->_method >> this->_uri >> this->_httpVersion;
	}

	std::string headerLine;
	while (std::getline(ss, headerLine) && headerLine != "\r") {
		std::istringstream headerLineStream(headerLine);
		std::string headerName;
		std::string headerValue;

		std::getline(headerLineStream, headerName, ':');
		std::getline(headerLineStream, headerValue);

		headerValue.erase(0, headerValue.find_first_not_of(' '));
		headerValue.erase(headerValue.find_last_not_of(' ') + 1);

		setHeader(headerName, headerValue);
	}
}
void Request::setHeader(std::string const &name, std::string const &value) {
	this->_headers[name] = value;
}
std::string Request::toString() const {
	std::ostringstream oss;
	oss << this->_method << " " << this->_uri << " " << this->_httpVersion << "\r\n";
	std::map<std::string, std::string>::const_iterator it = _headers.begin();
	for (; it != this->_headers.end(); it++) {
		oss << it->first << ": " << it->second;
	}
	oss << "\r\n";
	return oss.str();
}

std::ostream &operator<<(std::ostream &os, Request const &ref) {
	os << ref.toString();
	return os;
}
