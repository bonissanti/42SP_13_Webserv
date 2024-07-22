/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brunrodr <brunrodr@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/11 16:47:10 by brunrodr          #+#    #+#             */
/*   Updated: 2024/07/21 23:02:47 by brunrodr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Parser.hpp"

int main(int argc, char** argv)
{
    Server server;

    try {
        if (argc != 2)
            throw Parser::exception(RED "Error: invalid number of arguments" RESET);

        Parser parsing(argv[1]);
        server.creatingServers(parsing.getNumServers(), parsing.getLines());
    }
    catch (const Parser::exception& e) {
        std::cerr << e.what() << '\n';
    }
    //server.startServer(); cria o socket, verifica se a porta está em uso com setsockopt, chama bind e listen (se ok)
}
