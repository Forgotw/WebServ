/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsohler <lsohler@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 20:14:32 by lsohler           #+#    #+#             */
/*   Updated: 2024/06/01 17:04:17 by lsohler          ###   ########.fr       */
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

class CgiHandler {

	private:
		CgiHandler() {}
		~CgiHandler() {}

	public:
		static std::string	handleCGI(const Location* foundLocation, const Location* cgiLocation, std::string responseFilePath, const Request& request, const ServerConfig* config);

};
