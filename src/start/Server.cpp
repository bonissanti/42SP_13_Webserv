/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brunrodr <brunrodr@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/18 12:21:41 by brunrodr          #+#    #+#             */
/*   Updated: 2024/07/18 14:13:52 by brunrodr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/defines.h"

Server::Server(std::vector<std::string> lines)
{
	for (size_t i = 0; i < lines.size(); i++)
	{
		if (lines[i].substr(0, 6) == "listen")
			addPort(strtoi(lines[i].substr(7)));
	}
}

Server::~Server()
{

}

void	Server::addPort(int port)
{
	// if (port.compare() < 1024 || port > 49151)
	std::cout << port << std::endl;
		
}