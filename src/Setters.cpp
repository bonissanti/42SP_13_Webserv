// /* ************************************************************************** */
// /*                                                                            */
// /*                                                        :::      ::::::::   */
// /*   Setters.cpp                                        :+:      :+:    :+:   */
// /*                                                    +:+ +:+         +:+     */
// /*   By: brunrodr <brunrodr@student.42sp.org.br>    +#+  +:+       +#+        */
// /*                                                +#+#+#+#+#+   +#+           */
// /*   Created: 2024/07/21 22:32:48 by brunrodr          #+#    #+#             */
// /*   Updated: 2024/07/22 17:09:32 by brunrodr         ###   ########.fr       */
// /*                                                                            */
// /* ************************************************************************** */

// #include "../include/Utils.hpp"
// #include "../../include/Server.hpp"

// void Server::setListen(int port)
// {
//     try {
//         if (port < 1024 || port > 49151)
//             throw Server::exception(RED "Error: port number outside of the range. e.g.: 1024 ~ 49151");
//     }
//     catch (const std::exception& e) {
//         std::cerr << e.what() << '\n';
//     }
//     _listen = port;
// }

// void Server::setServerName(std::string name)
// {
//     _server_name = name;
// }

// void Server::setHost(std::string host)
// {
//     _host = host;
// }

// std::string setRoot(std::string root)
// {
//     if (root[0] != '/')
//         throw Server::exception(RED "Error: misformatted root path, please use '/path'" RESET);
//     if (root.substr(root.length() - 1) != "/")
//         root.insert(root.end(), '/');
//     return (root);
// }

// void Server::setBodySize(std::string size)
// {
//     std::string unit;
//     size_t pos = size.find_first_of("kbKBmbMBgbGB");

//     unit = size.substr(pos);
//     try {
//         switch (unit[0]) {
//             case 'k':
//             case 'K':
//                 _client_max_body_size = strtoi(size.substr(0, pos));
//                 break;

//             case 'm':
//             case 'M':
//                 _client_max_body_size = strtoi(size.substr(0, pos)) * MB;
//                 break;

//             case 'g':
//             case 'G':
//                 _client_max_body_size = strtoi(size.substr(0, pos)) * GB;
//                 break; 

//             default:
//                 throw Server::exception(RED "Error: unit not recognized" RESET);
//         }
//     }
//     catch (const std::exception& e) {
//         std::cout << e.what() << std::endl;
//     }
// }

// std::string setMethods(std::string methods)
// {
//     std::string allow_method;
//     std::stringstream ss(methods);

//     while (ss >> allow_method) {
//         if (allow_method == "get" || allow_method == "GET")
//             continue;

//         else if (allow_method == "post" || allow_method == "POST")
//             continue;

//         else if (allow_method == "delete" || allow_method == "DELETE")
//             continue;
//         else
//             throw std::invalid_argument(RED "Error: invalid method" RESET);
//     }
//     return (methods);
// }

// std::string setIndex(std::string index)
// {
//     if (index[0] == '/')
//         index.erase(index.begin());
//     if (index.substr(index.length() - 1) == "/")
//         index.erase(index.length() - 1);
//     return (index);
// }

// std::string setCGI(std::string file)
// {
//     if (file.substr(file.find_last_of(".") + 1) != ".py")
//         throw std::invalid_argument(RED "Error: file extension is not '.py'" RESET);

//     if (file[0] == '/')
//         file.erase(file.begin());
//     if (file.substr(file.length() - 1) == "/")
//         file.erase(file.length() - 1);
//     return (file);
// }

// std::string setRedirect(std::string url)
// {
//     if (url[0] == '/' || url.empty())
//         throw Server::exception(RED "Error: this is not a valid url" RESET);
//     return (url);
// }

// static int checkInsideRoute(std::string config)
// {
//     std::string type[] = {"autoindex", "root", "allow_methods", "index", "cgi", "_redirect"};  // adicionar outros

//     for (int i = 0; i < 6; i++)
//         if (config == type[i])
//             return (i);
//     return (-1);
// }

// void Server::setRoute(std::vector<std::string> lines, size_t& i)
// {
//     static int routeIndex;

//     _route.push_back(createRoute()); /*ver se na 42 cada posição do vector com o route
//     funcionam de forma independente, mas acredito que esteja ok */

//     for (; i < lines.size(); i++) {
//         std::string key, value;
//         std::stringstream ss(lines[i]);

//         if (getline(ss, key, '=') && getline(ss, value)) {
//             switch (checkInsideRoute(trim(key))) {
//                 case AUTOINDEX:
//                     _route[routeIndex]._autoIndex = (trim(value) == "on") ? true : false;
//                     break;

//                 case RROOT:
//                     _route[routeIndex]._root = setRoot(trim(value));
//                     break;

//                 case AMETHODS:
//                     _route[routeIndex]._allowMethods = setMethods(trim(value));
//                     break;

//                 case INDEX:
//                     _route[routeIndex]._index = setIndex(trim(value));
//                     break;

//                 case CGI:
//                     _route[routeIndex]._cgi = setCGI(trim(value));
//                     break;

//                 case REDIRECT:
//                     _route[routeIndex]._redirect = setRedirect(trim(value));
//                     break;
//             }
//         }
//         else if (lines[i] == "}")
//             break;
//     }
//     ++routeIndex;
// }