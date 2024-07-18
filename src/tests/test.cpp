/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brunrodr <brunrodr@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/11 16:47:15 by brunrodr          #+#    #+#             */
/*   Updated: 2024/07/18 15:42:51y brunrodr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/defines.h"

#define SERVER_PORT 18000

int	main(void)
{
	int n;
	int	socketFd;
	int connFd;
	struct sockaddr_in	servaddress;

	uint8_t buff[1024];
	uint8_t recvline[1024];

	// Cria o file descriptor - socket
	socketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketFd == -1)
	{
		std::cout << "RIP" << std::endl;
		return (1);
	}
	bzero(&servaddress, sizeof(servaddress));
	
	//Popula struct sockaddr_in
	servaddress.sin_family = AF_INET; // AF_INET = ipv4
	servaddress.sin_addr.s_addr = htonl(INADDR_ANY); //INADDR_ANY ip address specified (default is localhost)
	servaddress.sin_port = htons(SERVER_PORT); // a random port, for test this is 18000

	//associa a struct sockaddr e o file descriptor do socket
	int test = socket(AF_INET, SOCK_STREAM, 0);


	int result = bind(socketFd, (struct sockaddr *) &servaddress, sizeof(servaddress));
	if (result < 0)
	{
		if (errno == EADDRINUSE){
			std::cout << "RIP 2" << std::endl;
			return (1);
		}
	}

	if ((listen(socketFd, 10)) < 0)
	{
		std::cout << "RIP 3" << std::endl;
		return (1);
	}


	while (true)
	{
		std::cout << "Waiting for a connection on port: " << SERVER_PORT << std::endl;
		connFd = accept(socketFd, (struct sockaddr *)NULL, NULL);

		memset(recvline, 0, 1024);

		while ((n = read(connFd, recvline, 1024 -1)) > 0)	
		{
			if (recvline[n - 1] == '\n')
				break ;
			memset(recvline, 0, 1024);
		}
		if (n < 0)
		{
			std::cout << "fueeen" << std::endl;
			return (1);
		}

		snprintf((char*)buff, sizeof(buff), "HTTP/1.1 200/OK\r\n\r\nHello");
		write(connFd, (char *)buff, strlen((char *)buff));
		close(connFd);
	}
	return (0);
}