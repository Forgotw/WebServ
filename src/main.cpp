/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsohler <lsohler@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/12 15:27:33 by lsohler           #+#    #+#             */
/*   Updated: 2024/04/12 15:45:45 by lsohler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "ServerConfig.hpp"
#include "Socket.hpp"
#include "Request.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

std::map<std::string, Server>	configFileParser(std::string filename);

int main(int ac, char **av, char **envp) {
	(void)envp;
	if (ac == 2) {
		std::map<std::string, Server>	servers = configFileParser(av[1]);
		for(std::map<std::string, Server>::iterator it = servers.begin(); it != servers.end(); it++) {
			std::cout << "Key: " << it->first << std::endl;
			it->second.getConfig().printServerConfig();
			std::cout << std::endl;
		}
	}

}
