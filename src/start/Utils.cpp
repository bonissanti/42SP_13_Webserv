/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brunrodr <brunrodr@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/16 18:02:18 by brunrodr          #+#    #+#             */
/*   Updated: 2024/07/16 18:49:32 by brunrodr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Parser.hpp"

std::string	trim(std::string str)
{
	//trim right
	size_t pos = str.find_last_not_of(" \t\n");
	str.erase(pos + 1);

	//trim left
	pos = str.find_first_not_of(" \t\n"); 
	str.erase(0, pos);
	return (str);
}

bool	Parser::checkBrackets(std::vector<std::string> line)
{
	bool	bracketsOpen = false;

	for (std::vector<std::string>::iterator vecIt = line.begin(); vecIt != line.end(); ++vecIt)
	{
		for (std::string::iterator it = vecIt->begin(); it != vecIt->end(); ++it)
		{
			if (*it == '{')
			{
				if (bracketsOpen)
					throw Parser::exception(RED "Error: Nested braces are not allowed" RESET);
				bracketsOpen = true;
			}
			else if (*it == '}')
			{
				if (!bracketsOpen)
					throw Parser::exception(RED "Error: Closing brace without opening brace" RESET);	
				bracketsOpen = false;
			}
		}
	}	
	return (bracketsOpen);
}

void	Parser::analyzeConfig(std::vector<std::string> line)
{
	int numServers = 0;

	for (size_t i = 0; i < line.size(); i++)
	{
		if (line[i].substr(0, 6) == "server")
			numServers++;
	}
	std::cout << "numServers: " << numServers << std::endl;
}