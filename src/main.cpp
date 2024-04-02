/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsohler <lsohler@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/11 15:54:52 by lsohler           #+#    #+#             */
/*   Updated: 2024/04/02 15:29:38 by lsohler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"
#include "ServerConfig.hpp"
#include "Socket.hpp"

// std::vector<std::string>	configFileTokenizer(std::string filename);
std::map<std::string, WebServ>	configFileParser(std::string filename);

#include <iostream>
#include <cstring>
#include <netinet/in.h>
#include <sstream>
#include <bitset>
#include <arpa/inet.h>

void convertIPtoBinary(const std::string& ipAddress, struct in_addr& result) {
	uint32_t binaryIP = 0;
	uint32_t octet = 0;
	int shift = 24;
	
	for (size_t i = 0; i < ipAddress.size(); ++i) {
		if (ipAddress[i] == '.') {
			if (octet > 255) {
				std::cerr << "Adresse IP invalide." << std::endl;
				return;
			}
			binaryIP |= (octet << shift);
			octet = 0;
			shift -= 8;
		}
		else if (isdigit(ipAddress[i])) {
			octet = octet * 10 + (ipAddress[i] - '0');
		}
		else {
			std::cerr << "Adresse IP invalide." << std::endl;
			return;
		}
	}
	if (octet > 255) {
		std::cerr << "Adresse IP invalide." << std::endl;
		return;
	}
	binaryIP |= (octet << shift);
	result.s_addr = htonl(binaryIP);
}

std::string uint32ToIP(uint32_t ip) {
	std::ostringstream oss;
	oss << ((ip >> 24) & 0xFF) << '.'
		<< ((ip >> 16) & 0xFF) << '.'
		<< ((ip >> 8) & 0xFF) << '.'
		<< (ip & 0xFF);
	return oss.str();
}

// int main(int ac, char **av) {
// 	(void)ac;
// 	(void)av;
// 	const char *ipv4Str = "199872.168.1.1";
// 	struct in_addr ipv4Addr;

// 	convertIPtoBinary(ipv4Str, ipv4Addr);
// 	if (ipv4Addr.s_addr == 0) {
// 		std::cerr << "Conversion de l'adresse IP a échoué." << std::endl;
// 		return 1;
// 	}
// 	std::cout << "Adresse IP en binaire de " << ipv4Str << " : " << ntohl(ipv4Addr.s_addr) << std::endl;
// 	std::cout << "Ip Reel: " << uint32ToIP(ntohl(ipv4Addr.s_addr)) << std::endl;
// 	return 0;
// }

int main() {
    Socket serverSocket(8080);

    // Attendre et accepter les connexions entrantes
    while (true) {
        int clientSocket = serverSocket.acceptConnection();
		std::cout << "clientSocket: " << clientSocket << std::endl;
		std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
		response += "<html><body><h1>On va fumer ?!</h1></body></html>";

		send(clientSocket, response.c_str(), response.size(), 0);

		// Fermer la connexion avec le client
		close(clientSocket);
	}

	return 0;
}
