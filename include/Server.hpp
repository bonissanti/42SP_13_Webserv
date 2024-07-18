/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brunrodr <brunrodr@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/18 13:59:37 by brunrodr          #+#    #+#             */
/*   Updated: 2024/07/18 13:59:39 by brunrodr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

// #include "Client.hpp"
#include "defines.h"
#include "ServerConfig.hpp"
#include "Request.hpp"
// #include "Response.hpp"

class Server {
    public:
    Server(std::vector<std::string> lines);
    ~Server();
    void start();
	void	addPort(int port);

    private:
    void loadConfig(const std::string& config_path);
    void handleClient(int client_socket);

	class exception : public std::exception
	{
		private:
		std::string msg;

		public:
		exception(const std::string& msg);
		virtual ~exception() throw();
		virtual const char* what() const throw();
	};

    // Config              config_;
    // int                 server_socket_;
    // std::vector<Client> clients_;
};

#endif  // SERVER_HPP
