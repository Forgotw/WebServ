/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lray <lray@student.42lausanne.ch >         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/12 15:27:33 by lsohler           #+#    #+#             */
/*   Updated: 2024/04/12 18:19:07 by lray             ###   ########.fr       */
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
