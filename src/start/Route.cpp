/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brunrodr <brunrodr@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/18 19:40:58 by brunrodr          #+#    #+#             */
/*   Updated: 2024/07/19 18:03:39 by brunrodr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Route.hpp"

/* Route config of the server */

t_route	initRoute(void)
{
	t_route	route;

	route.autoindex = false;
	route.root = "/www";
	route.allow_methods = "GET POST DELETE";
	route.index = "index.html";
	route.cgi_path = "/usr/bin/python3 /bin/bash";
	return (route); 
}