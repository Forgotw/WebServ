/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsohler <lsohler@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/12 15:27:33 by lsohler           #+#    #+#             */
/*   Updated: 2024/04/13 18:57:47 by lsohler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

#include <iostream>

std::vector<ServerConfig>	configFileParser(std::string filename);

int main(int ac, char **av) {
	(void)ac;
	try {
		std::vector<ServerConfig> serverConfigVector = configFileParser(av[1]);
		WebServ master(serverConfigVector);
		master.start();
	} catch (std::exception &err) {
		std::cerr << "[!] " << err.what() << std::endl;
	}
	return 0;
}
// #include <unistd.h>
// #include <fstream>
// #include <string>

// bool fuckingCanOpenFile(const std::string& filename) {
//     char cwd[1024];
//     if (getcwd(cwd, sizeof(cwd)) == NULL) {
//         std::cerr << "Erreur lors de la récupération du répertoire de travail actuel." << std::endl;
//         return false;
//     }

//     std::string fullPath = std::string(cwd) + "/" + filename;
//     std::ifstream file(fullPath.c_str());
// 	std::cout << "Trying to open: " << fullPath.c_str() << std::endl;
    
//     if (!file.is_open()) {
//         std::cerr << "Erreur : Impossible d'ouvrir le fichier '" << fullPath << "'." << std::endl;
//         if (file.fail()) {
//             std::cerr << "Echec lors de l'ouverture du fichier." << std::endl;
//         }
//         return false;
//     }

//     // Lecture simple pour vérifier le contenu du fichier
//     std::string line;
//     while (getline(file, line)) {
//         std::cout << line << std::endl;
//     }

//     file.clear();  // Réinitialiser l'état d'erreur
//     file.close();
    
//     return true;
// }

// int main() {
// 	fuckingCanOpenFile("www/test/test.html");
// }
