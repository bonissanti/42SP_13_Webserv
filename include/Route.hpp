/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brunrodr <brunrodr@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/19 15:27:29 by brunrodr          #+#    #+#             */
/*   Updated: 2024/07/19 18:03:50 by brunrodr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROUTE_HPP
# define ROUTE_HPP

#include <map>
#include <string>

typedef struct s_route
{
	bool	autoindex;
	std::string	root;
	std::string	allow_methods;
	std::string	index;
	std::string	cgi_path;
	std::string	cgi_ext;
}	t_route;

t_route	initRoute(void);

#endif