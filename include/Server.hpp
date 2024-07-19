/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brunrodr <brunrodr@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/18 13:59:37 by brunrodr          #+#    #+#             */
/*   Updated: 2024/07/19 18:45:36 by brunrodr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Route.hpp"
#include "Config.hpp"
#include "Request.hpp"
#include "defines.h"

typedef enum	s_error_page
{
	ERROR_NOT_FOUND = 404 // talvez pensar num nome mais curto kkk e adicionar outros erros
}	t_error_page;

class Server
{
    private:
	int	_listen;
	std::string	_server_name;
	std::string _host;
	std::string _root;
	std::string _max_body_size;
	std::vector<t_error_page> _error_page;
	std::vector<t_route> _route;

	public:
	Server(void);
	~Server();	

	int		addPort(int port);
	void	setDefaultServer(void);
	void	creatingServers(int numServers, std::vector<std::string> lines);
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
