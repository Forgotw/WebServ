/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsohler <lsohler@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/02 15:00:41 by lsohler           #+#    #+#             */
/*   Updated: 2024/04/02 15:46:05 by lsohler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

Socket::Socket(int port) {
	// Création du socket
	_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverSocket == -1) {
		std::cerr << "Erreur lors de la création du socket." << std::endl;
		exit(EXIT_FAILURE);
	}

	// Configuration de l'adresse du serveur
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = INADDR_ANY;
	_address.sin_port = htons(port);

	// Attachement du socket à l'adresse et au port
	if (bind(_serverSocket, (struct sockaddr *)&_address, sizeof(_address)) == -1) {
		std::cerr << "Erreur lors de la liaison du socket à l'adresse et au port." << std::endl;
		exit(EXIT_FAILURE);
	}

	// Mise en attente des connexions entrantes
	if (listen(_serverSocket, 5) == -1) {
		std::cerr << "Erreur lors de la mise en attente des connexions entrantes." << std::endl;
		exit(EXIT_FAILURE);
	}
}

int	Socket::acceptConnection() {
	int clientSocket;
	struct sockaddr_in clientAddress;
	socklen_t clientAddressLength = sizeof(clientAddress);

	// Accepter la connexion entrante
	clientSocket = accept(_serverSocket, (struct sockaddr *)&clientAddress, &clientAddressLength);
	if (clientSocket == -1) {
		std::cerr << "Erreur lors de l'acceptation de la connexion entrante." << std::endl;
		exit(EXIT_FAILURE);
	}

	std::cout << "Connexion acceptée depuis " << inet_ntoa(clientAddress.sin_addr) << ":" << ntohs(clientAddress.sin_port) << std::endl;

	return clientSocket;
}
