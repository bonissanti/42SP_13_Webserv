/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brunrodr <brunrodr@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/16 18:53:22 by brunrodr          #+#    #+#             */
/*   Updated: 2024/07/18 19:42:54 by brunrodr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/defines.h"

/* Basic config of the server */

Config::Config() {
	_config["server_name"] = "default";
	_config["listen"] = "1024";
	_config["host"] = "localhost";
	_config["error_page"] = "error_pages/404.html";
	_config["client_max_body_size"] = "30MB";
	_config["root"] = "/www";
};
Config::~Config(){};


// server {
//     listen 8002;
// 	server_name localhost;
//     host 127.0.0.1;
//     root docs/fusion_web/;
//     # client_max_body_size 3000000;
// 	index index.html;
//     error_page 404 error_pages/404.html;

//     location / {
//         allow_methods  DELETE POST GET;
//         autoindex off;
//     }
    
//     location /tours {
//         autoindex on;
//         index tours1.html;
//         allow_methods GET POST PUT HEAD;
//     }

// 	location /red {
// 		return /tours;
// 	}

//     location /cgi-bin {
//         root ./;
//         allow_methods GET POST DELETE;
//         index time.py;
//         cgi_path /usr/bin/python3 /bin/bash;
//         cgi_ext .py .sh;
//     }
// }