#ifndef CONFIG_HPP
#define CONFIG_HPP

#define PORT 8080

#include <map>
#include <string>
#include "defines.hpp"
/* Após o parser terminar, é chamado o Config, ele vai analisar cada linha
presente no vector e irá 'setar' a classe de acordo com o que estava no arquivo
de configuração */

class Config {
    public:
        map<string, string> _config;
        Config();
        ~Config();
};

// Retirado do arquivo config (aquele de sugestão do gpt, caso precise)
// void        loadFromFile(const string& filepath);
// int         getPort() const;
// string getDocumentRoot() const;

#endif