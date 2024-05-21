/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsohler <lsohler@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 20:14:32 by lsohler           #+#    #+#             */
/*   Updated: 2024/05/13 20:40:16 by lsohler          ###   ########.fr       */
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
		static std::string	handleCGI(unsigned int* uiStatusCode, const Location* foundLocation, std::string responseFilePath, const Request& request, const ServerConfig* config);

};
