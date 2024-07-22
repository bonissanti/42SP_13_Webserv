/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser-Utils.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brunrodr <brunrodr@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/16 18:02:18 by brunrodr          #+#    #+#             */
/*   Updated: 2024/07/22 11:57:15 by brunrodr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Parser.hpp"
#include "../../include/Minilib.hpp"

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
	int nestedServer = 0;
	bool minimumReq;

	for (size_t i = 0; i < line.size(); i++)
	{
		if (line[i].empty())
			continue ;
		if (line[i].substr(0, 6) == "server")
		{
			_numServers++;
			nestedServer++;
			if (nestedServer > 1)
				throw Parser::exception(RED "Error: Nested servers are not allowed" RESET);
			minimumReq = false;
		}
		if (line[i].substr(0, 5) == "route")
			_numRoute++;
		else if (line[i].substr(0, 6) == "listen")
			minimumReq = true;
		else if (line[i].substr(0, 1) == "}")
			if (nestedServer > 0)
				nestedServer--;
	}
	if (!minimumReq)
		throw Parser::exception(RED "Error: port number (listen <number>) not informed in config file" RESET);
	std::cout << _numRoute << std::endl;
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

	if (isEmpty(file)){
		file.close();
		throw Parser::exception(RED "Error: config file is empty or doesn't exist" RESET);
	}
	// analyze first line
	getline(file, temp);
	_lines.push_back(trim(temp));
	if (_lines[0].substr(0, 6) != "server"){
		file.close();
		throw Parser::exception(RED "Error: misformatted config file" RESET);
	}

	// if first line okay, start to analyze other lines
	while (getline(file, temp))
		_lines.push_back(trim(temp));

	if (!Parser::checkBrackets(_lines)){
		file.close();
	}
	validConfigFile(_lines);
}