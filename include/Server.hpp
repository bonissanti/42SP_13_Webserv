/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brunrodr <brunrodr@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/18 13:59:37 by brunrodr          #+#    #+#             */
/*   Updated: 2024/07/19 19:03:34 by brunrodr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Config.hpp"
#include "Request.hpp"
#include "defines.hpp"

typedef enum s_error_page {
    ERROR_NOT_FOUND = 404  // TODO: adicionar outros erros
} t_error_page;

typedef struct s_route {
        bool        autoindex;
        std::string root;
        std::string allow_methods;
        std::string index;
        std::string cgi_path;
        std::string cgi_ext;
} t_route;

class Server {
    private:
        int                       _listen;
        std::string               _server_name;
        std::string               _host;
        std::string               _root;
        std::string               _max_body_size;
        std::string               _client_max_body_size;
        std::vector<t_error_page> _error_page;
        std::vector<t_route>      _route;

    public:
        Server(void);
        ~Server();

        t_route createRoute();
        int     setListen(int port);
        void    setDefaultServer(void);
        void    creatingServers(int numServers, std::vector<std::string> lines);
        // void start(); // void loadConfig(const std::string& config_path);
        // void handleClient(int client_socket);
        // Config              config_;

        class exception : public std::exception {
            private:
                std::string msg;

            public:
                exception(const std::string& msg);
                virtual ~exception() throw();
                virtual const char* what() const throw();
        };
};

#endif  // SERVER_HPP
