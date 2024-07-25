/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StartServer.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brunrodr <brunrodr@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/22 18:21:01 by brunrodr          #+#    #+#             */
/*   Updated: 2024/07/24 16:18:06by brunrodr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"

void	Server::startServer(std::vector<Server> servers)
{
	int	inUse = 1;
	struct sockaddr_in serverAddr;

	bzero(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	for (size_t i = 0; i < servers.size(); i++)
	{
		serverAddr.sin_port = htons(servers[i]._listen);
		servers[i]._socketFd = socket(AF_INET, SOCK_STREAM, 0);
	
		if (setsockopt(servers[i]._socketFd, SOL_SOCKET, SO_REUSEADDR, &inUse, sizeof(int)) == -1)
			throw Server::exception(RED "Error: setsockopt failed" RESET);

		if (bind(servers[i]._socketFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
			throw Server::exception(RED "Error: bind failed" RESET);

		if (listen(servers[i]._socketFd, 10) < 0)
			throw Server::exception(RED "Error: listen failed" RESET);
	}
}

static int	acceptNewConnection(int serverSocket)
{
	int clientSocket;
	if (clientSocket = accept(serverSocket, (struct sockaddr *)NULL, NULL))
	{
		perror("accept failed");
		return (-1);
	}
	return (clientSocket);
}

void Server::setupPolls(std::vector<Server> servers)
{
	fd_set	socketSet;
	struct timeval setTimeout;

	FD_ZERO(&socketSet);
	for (size_t i = 0; i < servers.size(); i++)
		FD_SET(servers[i]._listen, &socketSet);
	
	setTimeout.tv_sec = 100000000;
	setTimeout.tv_usec = 0;

	// for (std::vector<Servers>::iterator listn = servers.begin(); listn = servers.end(); ++listn)


}