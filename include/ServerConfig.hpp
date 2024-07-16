/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brunrodr <brunrodr@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/16 18:51:27 by brunrodr          #+#    #+#             */
/*   Updated: 2024/07/16 18:53:10 by brunrodr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

class ServerConfig
{
	private:
	int id;
	int port;
	// adicionar outras coisas

	public:
	ServerConfig();
	~ServerConfig();
};

#endif