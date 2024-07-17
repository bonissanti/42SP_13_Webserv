/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brunrodr <brunrodr@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/15 20:14:26 by brunrodr          #+#    #+#             */
/*   Updated: 2024/07/17 18:08:37 by brunrodr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Parser.hpp"

Parser::Parser(std::string arg){
	debugMode("<Parser> Default Constructor called");

	if (checkFileName(arg))
		throw Parser::exception(RED "Error: incorrect config file" RESET);
	analyzeConfig(arg);
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