/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WEBSERVER.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brunrodr <brunrodr@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/15 13:29:56 by brunrodr          #+#    #+#             */
/*   Updated: 2024/07/15 13:54:16 by brunrodr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sstream>
#include <exception>
#include "defines.h"

#define PORT 8080

class Webserver
{
	private:
	/* data */

	public:
	Webserver(char **argv);
	~Webserver();
	Webserver(const Webserver& toCopy);
	Webserver& operator=(const Webserver& toCopy);

	class exception : public std::exception
	{
		private:
		std::string msg;

		public:
		exception(const std::string& msg);
		virtual ~exception();
		virtual const char* what() const throw();
	};
};

void	debugMode(const std::string& msg);
std::string binToHex(const char* digits);

#endif