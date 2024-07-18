/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brunrodr <brunrodr@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/11 16:47:10 by brunrodr          #+#    #+#             */
/*   Updated: 2024/07/18 14:10:04 by brunrodr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Parser.hpp"
#include "../include/Server.hpp"

int main(int argc, char **argv)
{
	try
	{
		if (argc != 2)
			throw Parser::exception(RED "Error: invalid number of arguments" RESET);
		Parser parsing(argv[1]);
		Server server(parsing.getLines());
	}
	catch(const Parser::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}
