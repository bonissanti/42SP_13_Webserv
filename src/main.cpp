/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brunrodr <brunrodr@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/11 16:47:10 by brunrodr          #+#    #+#             */
/*   Updated: 2024/07/19 18:40:39 by brunrodr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/defines.h"


int main(int argc, char **argv)
{
	Server server;

	try
	{
		if (argc != 2)
			throw Parser::exception(RED "Error: invalid number of arguments" RESET);

		Parser parsing(argv[1]);
		server.creatingServers(parsing.getNumServers(), parsing.getLines());
	}
	catch(const Parser::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}
