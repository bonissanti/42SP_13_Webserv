/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brunrodr <brunrodr@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/18 12:21:41 by brunrodr          #+#    #+#             */
/*   Updated: 2024/07/19 19:08:15 by brunrodr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"

Server::Server()
{
    t_route      routeDefault;
    t_error_page errors = ERROR_NOT_FOUND;

    _listen = 8080;
    _server_name = "default";
    _host = "localhost";
    _max_body_size = "30MB";
    _client_max_body_size = "30MB";
    _root = "30MB";

    routeDefault = createRoute();
    _route.push_back(routeDefault);
    _error_page.push_back(errors);
}

Server::~Server() {}

t_route Server::createRoute()
{
    t_route route;

    route.autoindex = false;
    route.root = "/www";
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

int Server::setListen(int port)
{
    try {
        if (port < 1024 || port > 49151)
            throw Server::exception(RED
                                    "Error: port number outside of the range. e.g.: 1024 ~ 49151");
        // TODO: incluir validação se porta está sendo utilizada
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
    return (port);
}

void Server::creatingServers(int numServers, std::vector<std::string> lines)
{
    Server* arrayServer = new Server[numServers]();
    (void)lines;

    // teste para ver se todos os servidores foram preenchidos pelo default
    // for (int i = 0; i < numServers; i++)
    // 	std::cout << arrayServer[i]._listen << std::endl;

    for (size_t i = 0; i < lines.size(); i++) {
        std::string       key, value;
        std::stringstream ss(lines[i]);

        if (getline(ss, key, '=') && getline(ss, value)) {
            /*
            trim
            if (key == "listen")
                arrayServer[i].setListen()
            else if (key == "server_name")
                arrayServer[i].setServerName()
            else if (key == "host")
                arrayServer[i]. setHost()

            ouuu com switch case, com uma função auxiliar que retorna um index para cada string
            e chama o 'case' para setar aquela key especifica

            switch (getKeyIndex(key))
            {
                case LISTEN: enum ou 0 diretão
                arrayServer[i].setListen();

                case SERVER_NAME:
                arrayServer[i].setServerName();
            }
            */
        }
    }
}