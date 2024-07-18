/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brunrodr <brunrodr@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/18 12:21:41 by brunrodr          #+#    #+#             */
/*   Updated: 2024/07/18 17:00:24 by brunrodr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/defines.h"

Server::Server(std::vector<std::string> lines)
{
	for (size_t i = 0; i < lines.size(); i++)
	{
		Config configs;
		if (lines[i].substr(0, 6) == "listen")
			configs._port = addPort(strtoi(lines[i].substr(7)));
		_servers.push_back(configs);
	}
}

Server::~Server()
{

}

Server::Server::exception::exception(const std::string& msg) : msg(msg){}
Server::Server::exception::~exception() throw(){}
const char* Server::Server::exception::what() const throw() 
{
	return (this->msg.c_str());
}

int	Server::addPort(int port)
{
	try
	{
		if (port < 1024 || port > 49151)
			throw Server::exception(RED "Error: port number outside of the range. e.g.: 1024 ~ 49151");
		// incluir validação se porta está sendo utilizada
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return (port);
}
