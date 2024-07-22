/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brunrodr <brunrodr@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/18 12:21:41 by brunrodr          #+#    #+#             */
/*   Updated: 2024/07/22 12:13:13 by brunrodr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"
#include "../../include/Minilib.hpp"

static int getKeyIndex(std::string key);

Server::Server()
{
    t_error_page errors = ERROR_NOT_FOUND;

    _listen = 8080;
    _client_max_body_size = 30 * MB;
    _server_name = "default";
    _host = "localhost";
    _root = "/data/"; // = localhost/image.png -> /data/image.png

    _error_page.push_back(errors);
}

Server::~Server() {}

t_route Server::createRoute()
{
    t_route route;

    route.autoindex = false;
    route.root = "/data/"; // se não especificado, fica o mesmo do server
    route.allow_methods = "GET POST DELETE";
    route.index = "index.html";
    route.cgi_path = "/usr/bin/python3 /bin/bash";
    return (route);
}

Server::Server::exception::exception(const std::string& msg) : msg(msg) {}
Server::Server::exception::~exception() throw() {}

const char* Server::Server::exception::what() const throw()
{
    return (this->msg.c_str());
}

void Server::creatingServers(int numServers, std::vector<std::string> lines)
{
    int serverIndex = -1;
    Server* arrayServer = new Server[numServers](); // new ou um container pra guardar servers (fora dessa função)

	for (size_t i = 0; i < lines.size(); i++) {
		std::string       key, value;
		std::stringstream ss(lines[i]);

        if (lines[i].substr(0, 6) == "server"){
            serverIndex++;
            continue;
        }
		if (getline(ss, key, '=') && getline(ss, value) && serverIndex >= 0 && serverIndex < numServers) 
		{
		    switch (getKeyIndex(trim(key)))
		    {
		        case LISTEN:
		        arrayServer[serverIndex].setListen(strtoi(value));
				break;

				case SERVER_NAME:
				arrayServer[serverIndex].setServerName(value);
                break;

                case HOST:
                arrayServer[serverIndex].setHost(value);
                break;

                case ROOT:
                arrayServer[serverIndex].setRoot(value);
                break;
      
                case CLIENT_MBSIZE:
                arrayServer[serverIndex].setBodySize(trim(value));
                break;
		    }
		}
        else if (lines[i].substr(0, 5) == "route")
            arrayServer[serverIndex].setRoute(lines, i);
        else if (lines[i] == "}")
            break;
    }
}

static int getKeyIndex(std::string key)
{
	std::string types[] = {"listen", "server_name", "host", "root", "client_max_body_size"};

	for (size_t i = 0; i < 6; i++)
		if (key == types[i])
			return (i);
	return (-1);
}