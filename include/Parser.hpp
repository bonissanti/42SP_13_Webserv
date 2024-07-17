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


/* Classe de parser, basicamente ela valida o arquivo de configuração e popula
um vector com as linhas retiradas do arquivo de configuração. É este vector que
a classe ServerConfig utiliza para configurar o servidor */

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
	void	validConfigFile(std::vector<std::string> line);
	bool	isEmpty(std::ifstream& file);
	void	analyzeConfig(std::string arg);

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
std::string binToHex(const char* digits); // talvez seja util no futuro

#endif