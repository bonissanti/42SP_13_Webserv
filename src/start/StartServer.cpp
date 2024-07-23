/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StartServer.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brunrodr <brunrodr@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/22 18:21:01 by brunrodr          #+#    #+#             */
/*   Updated: 2024/07/23 11:46:08 by brunrodr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"

void	Server::startServer(std::vector<Server> servers)
{
	struct sockaddr_in serverAddr;

	bzero(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(servers[0]._listen);
	
	
}