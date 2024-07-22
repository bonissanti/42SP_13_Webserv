/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Setters.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brunrodr <brunrodr@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/21 22:32:48 by brunrodr          #+#    #+#             */
/*   Updated: 2024/07/22 12:26:46 by brunrodr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"
#include "../../include/Minilib.hpp"

void Server::setListen(int port)
{
    try {
        if (port < 1024 || port > 49151)
            throw Server::exception(RED"Error: port number outside of the range. e.g.: 1024 ~ 49151");
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
	_listen = port;
}

void    Server::setServerName(std::string name)
{
    _server_name = name;
}

void    Server::setHost(std::string host)
{
    _host = host;
}

void    Server::setRoot(std::string root)
{
    if (root[0] != '/')
        throw Server::exception(RED "Error: misformatted root path, please use '/path'" RESET);
    _root = root;
}

void    Server::setBodySize(std::string size)
{
    std::string unit;
    size_t pos = size.find_first_of("kbKBmbMBgbGB");

    unit = size.substr(pos);
    try
    {
        switch (unit[0])
        {
            case 'k':
            case 'K':
            _client_max_body_size = strtoi(size.substr(0, pos));
            break ;
    
            case 'm':
            case 'M':
            _client_max_body_size = strtoi(size.substr(0, pos)) * MB;
            break;
    
	    	case 'g':
            case 'G':
            _client_max_body_size = strtoi(size.substr(0, pos)) * GB;
            break ;
    
            default:
	    	throw Server::exception(RED "Error: unit not recognized" RESET);
        }
    }
    catch (const std::exception &e){
        std::cout << e.what() << std::endl;
    }
}

static int	checkInsideRoute(std::string config)
{
	std::string type[] = {"autoindex"}; // adicionar outros

	for (int i = 0; i < 1; i++)
		if (config == type[i])
			return (i);
	return (-1);
}

void	Server::setRoute(std::vector<std::string> lines, size_t& i)
{
    static int routeIndex;

    _route.push_back(createRoute()); /*ver se na 42 cada posição do vector com o route
    funcionam de forma independente, mas acredito que esteja ok */
    
	for ( ; i < lines.size(); i++)
	{
        std::string key, value;
        std::stringstream ss(lines[i]);

        if (getline(ss, key, '=') && getline(ss, value))
        {
            switch(checkInsideRoute(trim(key)))
            {
                case AUTOINDEX:
                _route[routeIndex].autoindex = (trim(value) == "on") ? true : false;
				break;

				/*
				case ROOT ou RROOT:
				_route[0].root = 
				break;

				case AMETHODS:
				_route[0].allow_methods = 
				break;
				
				case INDEX:
				_route[0].index = 
				break;

				case CGIPATH
				_route[0].cgi_path = 
				break;

				case CGIEXT
				_route[0].cgi_ext =
				break;
				*/
            }
        }
        else if (lines[i] == "}")
            break;
	}
    ++routeIndex;
}