/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brunrodr <brunrodr@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/18 13:59:37 by brunrodr          #+#    #+#             */
/*   Updated: 2024/07/18 17:08:37 by brunrodr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Config.hpp"
#include "Request.hpp"
#include "defines.h"

class Server
{
    private:
    std::vector<Config> _servers;

    public:
    Server(std::vector<std::string> lines);
    ~Server();

	int addPort(int port);

    // void start(); // void loadConfig(const std::string& config_path);
    // void handleClient(int client_socket);
    // Config              config_;

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

#endif  // SERVER_HPP
