/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lray <lray@student.42lausanne.ch >         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/12 15:27:33 by lsohler           #+#    #+#             */
/*   Updated: 2024/06/04 15:38:06 by lray             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

#include <iostream>
#include <string>

std::vector<ServerConfig>	configFileParser(std::string filename);

int main(int ac, char **av) {
	std::string configFilePath;
	if (ac < 2) {
		configFilePath = "conf/default.conf";
	} else {
		configFilePath = av[1];
	}
	try {
		std::vector<ServerConfig> serverConfigVector = configFileParser(configFilePath);
		WebServ master(serverConfigVector);
		master.start();
	} catch (std::exception &e) {
		std::cout << "[!] " << e.what() << "\n";
	}
	return 0;
}
