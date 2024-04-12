/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsohler <lsohler@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/10 11:20:53 by lsohler@stu       #+#    #+#             */
/*   Updated: 2024/04/12 15:21:31 by lsohler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Server.hpp"
#include "ServerConfig.hpp"

Server::Server(void)  {

}

Server::Server(Server const &other) : _config(other._config) {

}

Server::Server(ServerConfig &new_config) {
	_config = new_config;
}

Server::~Server(void) {

}

Server	&Server::operator=(Server const &other) {
	if (this != &other) {
		_config = other._config;
	}
	return *this;
}
