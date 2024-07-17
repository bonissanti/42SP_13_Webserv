/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brunrodr <brunrodr@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/16 18:02:18 by brunrodr          #+#    #+#             */
/*   Updated: 2024/07/17 18:33:53 by brunrodr         ###   ########.fr       */
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
	int	openBrackets = 0;
	int closeBrackets = 0;

	for (std::vector<std::string>::iterator vecIt = line.begin(); vecIt != line.end(); ++vecIt)
	{
		for (std::string::iterator it = vecIt->begin(); it != vecIt->end(); ++it)
		{
			if (*it == '{')
				openBrackets++;
			else if (*it == '}')
				closeBrackets++;
		}
	}
	if (openBrackets > closeBrackets)
		throw Parser::exception(RED "Error: Nested braces are not allowed" RESET);	
	else if (openBrackets < closeBrackets)
		throw Parser::exception(RED "Error: Closing brace without opening brance" RESET);
	return (true);
}

void	Parser::validConfigFile(std::vector<std::string> line)
{
	int numServers = 0; // temp
	int nestedServer = 0;
	bool minimumReq;

	for (size_t i = 0; i < line.size(); i++)
	{
		if (line[i].empty())
			continue ;
		if (line[i].substr(0, 7) == "server ")
		{
			numServers++;
			nestedServer++;
			if (nestedServer > 1)
				throw Parser::exception(RED "Error: Nested servers are not allowed" RESET);
			minimumReq = false;
		}
		else if (line[i].substr(0, 7) == "listen ")
			minimumReq = true;
		else if (line[i].substr(0, 1) == "}")
			if (nestedServer > 0)
				nestedServer--;
	}
	if (!minimumReq)
		throw Parser::exception(RED "Error: port number (listen <number>) not informed in config file" RESET);
	std::cout << "numServers: " << numServers << std::endl;
}

bool	Parser::checkFileName(std::string file)
{
	if (file.substr(file.find(".") + 1) == "conf")
		return (true);
	return (false);
}

bool	Parser::isEmpty(std::ifstream& file)
{
	return (file.peek() == std::ifstream::traits_type::eof());
}

void	Parser::analyzeConfig(std::string arg)
{
	std::string temp;
	std::ifstream file(arg.c_str());
	std::vector<std::string> line;

	if (isEmpty(file)){
		file.close();
		throw Parser::exception(RED "Error: config file is empty or doesn't exist" RESET);
	}
	// analyze first line
	getline(file, temp);
	line.push_back(trim(temp));
	if (line[0].substr(0, 7) != "server "){
		file.close();
		throw Parser::exception(RED "Error: misformatted config file" RESET);
	}

	// if first line okay, start to analyze other lines
	while (getline(file, temp))
		line.push_back(trim(temp));

	if (!Parser::checkBrackets(line)){
		file.close();
	}
	validConfigFile(line);
}