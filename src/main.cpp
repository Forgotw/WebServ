/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lray <lray@student.42lausanne.ch >         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/12 15:27:33 by lsohler           #+#    #+#             */
/*   Updated: 2024/04/12 16:48:19 by lray             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

#include <iostream>

int main() {
	try {
		WebServ master("127.0.0.1", "8080");
		master.start();
	} catch (std::exception &err) {
		std::cerr << "[!] " << err.what() << std::endl;
	}
	return 0;
}
