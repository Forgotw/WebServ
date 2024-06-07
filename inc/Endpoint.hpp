/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Endpoint.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsohler <lsohler@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 17:45:13 by lsohler           #+#    #+#             */
/*   Updated: 2024/05/21 17:50:34 by lsohler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

/*
**==========================
**		Include
**==========================
*/
#include "Request.hpp"
#include "ServerConfig.hpp"

class Endpoint {

    private:
        Endpoint();
        ~Endpoint();


    public:
        void postHardCodedEndpoint(const Request& request, const ServerConfig& server_config);
        void deleteHardCodedEndpoint(const Request& request, const ServerConfig& server_config);

}