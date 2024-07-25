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