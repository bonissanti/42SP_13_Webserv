// /* ************************************************************************** */
// /*                                                                            */
// /*                                                        :::      ::::::::   */
// /*   Server.cpp                                         :+:      :+:    :+:   */
// /*                                                    +:+ +:+         +:+     */
// /*   By: brunrodr <brunrodr@student.42sp.org.br>    +#+  +:+       +#+        */
// /*                                                +#+#+#+#+#+   +#+           */
// /*   Created: 2024/07/18 12:21:41 by brunrodr          #+#    #+#             */
// /*   Updated: 2024/07/22 18:08:57 by brunrodr         ###   ########.fr       */
// /*                                                                            */
// /* ************************************************************************** */

// #include "../include/Server.hpp"

// static int getKeyIndex(std::string key);

// Server::Server()
// {
//     status_request status = DEFAULT;

//     _listen = 8080;
//     _client_max_body_size = 30 * MB;
//     _server_name = "default";
//     _host = "localhost";
//     _root = "/data/";  // = localhost/image.png -> /data/image.png

//     _error_page.push_back(status);
// }

// Server::~Server() {}

// t_route Server::createRoute()
// {
//     t_route route;

//     route._autoIndex = false;
//     route._root = "/data/";  // se não especificado, fica o mesmo do server
//     route._allowMethods = "GET POST DELETE";
//     route._index = "index.html";
//     route._cgi = "";
//     return (route);
// }

// Server::Server::exception::exception(const std::string& msg) : msg(msg) {}
// Server::Server::exception::~exception() throw() {}

// const char* Server::Server::exception::what() const throw()
// {
//     return (this->msg.c_str());
// }

// std::vector<Server> Server::creatingServers(int numServers, std::vector<std::string> lines)
// {
//     int serverIndex = -1;
//     std::vector<Server> servers(numServers);

//     for (size_t i = 0; i < lines.size(); i++) {
//         std::string key, value;
//         std::stringstream ss(lines[i]);

//         if (lines[i].substr(0, 6) == "server") {
//             serverIndex++;
//             continue;
//         }
//         if (getline(ss, key, '=') && getline(ss, value) && serverIndex >= 0 && serverIndex < numServers) {
//             switch (getKeyIndex(Utils::trim(key))) {
//                 case LISTEN:
//                     servers[serverIndex].setListen(Utils::strtoi(value));
//                     break;

//                 case SERVER_NAME:
//                     servers[serverIndex].setServerName(value);
//                     break;

//                 case HOST:
//                     servers[serverIndex].setHost(value);
//                     break;

//                 case ROOT:
//                     servers[serverIndex]._root = setRoot(Utils::trim(value));
//                     break;

//                 case CLIENT_MBSIZE:
//                     servers[serverIndex].setBodySize(Utils::trim(value));
//                     break;
//             }
//         }
//         else if (lines[i].substr(0, 5) == "route")
//             servers[serverIndex].setRoute(lines, i);
//         else if (lines[i] == "}")
//             break;
//     }
//     return (servers);
// }

// static int getKeyIndex(std::string key)
// {
//     std::string types[] = {"listen", "server_name", "host", "root", "client_max_body_size"};

//     for (size_t i = 0; i < 6; i++)
//         if (key == types[i])
//             return (i);
//     return (-1);
// }

// // void	Server::startServer(std::vector<Server> servers)
// // {
// // 	struct sockaddr_in serverAddr;

// // 	serverAddr.sin_family = AF_INET;
// // 	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
// // 	// for ... servers
// // 	// serverAddr.sin_port = htons(servers[]._port);
// // 	// socket
// // 	//setsockopt
// // }