/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsohler <lsohler@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 20:14:32 by lsohler           #+#    #+#             */
/*   Updated: 2024/06/10 16:48:28 by lsohler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

/*
**==========================
**		Include
**==========================
*/
#include <string>
#include "ServerConfig.hpp"
#include "Request.hpp"
#include "Location.hpp"
#include "Server.hpp"
#include "Peer.hpp"

class CgiHandler {

	private:
		CgiHandler() {}
		~CgiHandler() {}

	public:
		static void         handleCGI(const Location* foundLocation, const Location* cgiLocation, std::string responseFilePath, Peer& peer, const Server* server);
        static std::string  ProcessCgiOutput(const Server* server, const std::string& cgiOutput);
};
