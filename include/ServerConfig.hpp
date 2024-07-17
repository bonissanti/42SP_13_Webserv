/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brunrodr <brunrodr@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/16 18:51:27 by brunrodr          #+#    #+#             */
/*   Updated: 2024/07/17 17:57:13 by brunrodr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

#define PORT 8080

/* Após o parser terminar, é chamado o ServerConfig, ele vai analisar cada linha
presente no vector e irá 'setar' a classe de acordo com o que estava no arquivo
de configuração */

class ServerConfig
{
	private:
	int _port;
	// adicionar outros atributos

	public:
	ServerConfig();
	~ServerConfig();

	// Retirado do arquivo config (aquele de sugestão do gpt, caso precise)
	// void        loadFromFile(const std::string& filepath);
	// int         getPort() const;
	// std::string getDocumentRoot() const;
};

#endif