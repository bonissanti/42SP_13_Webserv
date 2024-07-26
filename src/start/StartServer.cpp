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

void	Server::startServer(std::vector<Server>& servers)
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

		int flags = fcntl(servers[i]._socketFd, F_GETFL);
		if (flags < 0)
			throw Server::exception(RED "Error: fcntl failed" RESET);

		if (fcntl(servers[i]._socketFd, F_SETFL, flags | O_NONBLOCK) < 0)
			throw Server::exception(RED "Error: fcntl failed" RESET);

		if (bind(servers[i]._socketFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
		{
			if (errno == EADDRINUSE)
				throw Server::exception(RED "Error: <bind> port is in use by other server" RESET);
		}
		if (listen(servers[i]._socketFd, 10) < 0)
			throw Server::exception(RED "Error: listen failed" RESET);
	}
}

static void	acceptNewConnection(int serverSocket, std::vector<struct pollfd>& pollFds)
{
	int clientFd;
	socklen_t addrlen;
	struct	sockaddr_in	clientAddr;

	addrlen = sizeof(clientAddr);
	clientFd = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrlen);
	if (clientFd < 0)
		perror("Error: accept failed");
	std::cout << "Connection established successfully!" << std::endl; // log message

	struct pollfd commFd;

	commFd.fd = clientFd;
	commFd.events = POLLIN | POLLOUT;
	commFd.revents = 0;
	pollFds.push_back(commFd);
}

static void	readRequest(std::vector<struct pollfd>& pollFds, int i)
{
	char buffer[65535];
	static int a;
	ssize_t bytesReceived = recv(pollFds[i].fd, buffer, sizeof(buffer), 0);
	if (bytesReceived > 0)
	 	std::cout << ++a << std::endl;
		// std::cout << "Received from client: " << std::string(buffer, bytesReceived) << std::endl;
	else if (bytesReceived == 0)
		std::cout << "Connection closed" << std::endl;
	else
		perror("Error: recv failed");
	close (pollFds[i].fd);
	pollFds.erase(pollFds.begin() + i);
}

void Server::setupPolls(std::vector<Server> servers)
{
	int	returnValue;
	std::vector<struct pollfd> pollFds(servers.size());

	for (size_t i = 0; i < servers.size(); i++){
		pollFds[i].fd = servers[i]._socketFd;
		pollFds[i].events = POLLIN | POLLOUT;
	}
	while (true)
	{
		returnValue = poll(pollFds.data(), pollFds.size(), -1);
		switch (returnValue)
		{
			case 0:
			std::cout << "Error: poll Timeout" << std::endl;
			break;
		
			case -1:
			std::cout << "Error: poll failed" << std::endl;
			break;
	
			default:
			for (size_t i = 0; i < pollFds.size(); i++){
				if ((pollFds[i].revents & POLLIN) && (i < servers.size())){
					acceptNewConnection(pollFds[i].fd, pollFds);
				}
				else if (pollFds[i].revents & POLLIN)
					readRequest(pollFds, i);
			}
			break;
		}
	}
}