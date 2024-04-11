#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <map>
#include <ostream>

class Request {
public:
	Request(std::string const &buffer);
	~Request();

	std::string toString() const;

	std::string const &getMethod() const;
	std::string const &getUri() const;
	std::string const &getHttpVersion() const;
	std::map<std::string, std::string> const &getHeaders() const;

private:
	std::string _method;
	std::string _uri;
	std::string _httpVersion;
	std::map<std::string, std::string> _headers;

	void parseBuffer(std::string const &buffer);
	void setHeader(std::string const &name, std::string const &value);
};
std::ostream &operator<<(std::ostream &os, Request const &ref);

#endif
