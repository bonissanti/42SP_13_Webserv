/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brunrodr <brunrodr@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/11 16:47:10 by brunrodr          #+#    #+#             */
/*   Updated: 2024/07/22 18:09:46by brunrodr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Parser.hpp"

int main(int argc, char** argv)
{
    try {
        if (argc != 2)
            throw Parser::exception(RED "Error: invalid number of arguments" RESET);

        Parser parsing(argv[1]);
        std::vector<Server> servers = Server::creatingServers(parsing.getNumServers(), parsing.getLines());

        // while (true) um looping infinito aqui talvez?
        Server::startServer(servers);
        Server::setupPolls(servers);
    }
    catch (const Server::exception& e) {
        std::cerr << e.what() << '\n';
    }
    catch (const Parser::exception& e) {
        std::cerr << e.what() << '\n';
    }
}
