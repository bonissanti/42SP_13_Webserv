/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brunrodr <brunrodr@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/16 18:51:27 by brunrodr          #+#    #+#             */
/*   Updated: 2024/07/18 16:31:36 by brunrodr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#define PORT 8080

#include <map>
#include <string>

/* Após o parser terminar, é chamado o Config, ele vai analisar cada linha
presente no vector e irá 'setar' a classe de acordo com o que estava no arquivo
de configuração */

class Config 
{
	public:
	std::map<std::string, std::string> _config;
	Config();
	~Config();
};

// Retirado do arquivo config (aquele de sugestão do gpt, caso precise)
// void        loadFromFile(const std::string& filepath);
// int         getPort() const;
// std::string getDocumentRoot() const;

#endif