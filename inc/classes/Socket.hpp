/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsohler <lsohler@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/02 15:00:54 by lsohler           #+#    #+#             */
/*   Updated: 2024/04/02 15:28:50 by lsohler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

/*
**==========================
**		Include
**==========================
*/
#include <iostream>
#include <string>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class Socket {

	private:
		int					_serverSocket;
		struct	sockaddr_in	_address;

	public:
	Socket(int port);

	~Socket() {
		close(_serverSocket);
	}

	int	acceptConnection();
};
