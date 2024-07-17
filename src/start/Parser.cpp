/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brunrodr <brunrodr@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/15 20:14:26 by brunrodr          #+#    #+#             */
/*   Updated: 2024/07/17 15:05:00 by brunrodr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Parser.hpp"

Parser::Parser(std::string arg){
	debugMode("<Parser> Default Constructor called");

	if (checkFileName(arg))
		throw Parser::exception(RED "Error: incorrect config file" RESET);

	// start of function
	std::string temp;
	std::ifstream file(arg.c_str());
	std::vector<std::string> line;

	if (isEmpty(file)){
		file.close();
		throw Parser::exception(RED "Error: config file is empty" RESET);
	}
	// analyze first line
	getline(file, temp);
	line.push_back(temp);
	if (line[0].substr(0, 6) != "server"){
		file.close();
		throw Parser::exception(RED "Error: misformatted config file" RESET);
	}

	// if first line okay, start to analyze other lines
	while (getline(file, temp))
		line.push_back(trim(temp));

	if (Parser::checkBrackets(line)){
		file.close();
		return ;
	}
	analyzeConfig(line);
	// end of function
}

Parser::~Parser(){
	debugMode("<Parser> Destructor called");
}

Parser::Parser(const Parser& toCopy){
	debugMode("<Parser> Copy Constructor called");
		*this = toCopy;
}

Parser& Parser::operator=(const Parser& toCopy){
	debugMode("<Parser> Copy Assignment Operator called");
	(void)toCopy;
	return (*this);
}

// exception 
Parser::exception::exception(const std::string& msg) : msg(msg){};
Parser::exception::~exception() throw(){};
const char* Parser::exception:: what() const throw(){
	return (msg.c_str());
}