/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PARSER.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brunrodr <brunrodr@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/15 13:29:56 by brunrodr          #+#    #+#             */
/*   Updated: 2024/07/15 13:54:16 by brunrodr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sstream>
#include <exception>
#include <fstream>
#include <vector>
#include "defines.h"

#define PORT 8080

class Parser
{
	private:
	

	public:
	Parser(std::string arg);
	~Parser();
	Parser(const Parser& toCopy);
	Parser& operator=(const Parser& toCopy);

	bool	checkBrackets(std::vector<std::string> line);
	bool	checkFileName(std::string file);
	void	analyzeConfig(std::vector<std::string> line);
	bool	isEmpty(std::ifstream& file);

	class exception : public std::exception
	{
		private:
		std::string msg;

		public:
		exception(const std::string& msg);
		virtual ~exception() throw();
		virtual const char* what() const throw();
	};
};

std::string	trim(std::string str);
void	debugMode(const std::string& msg);
std::string binToHex(const char* digits);

#endif