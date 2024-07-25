#ifndef VALIDATE_HPP
#define VALIDATE_HPP

#include "defines.hpp"
#include "Utils.hpp"

/* Classe de parser, basicamente ela valida o arquivo de configuração e popula
um vector com as linhas retiradas do arquivo de configuração. É este vector que
a classe ServerConfig utiliza para configurar o servidor */

class Validate {
    private:
    	int _numRoute;
        std::vector<std::string> _lines;

    public:
        Validate(std::string arg);
        ~Validate();

        // validations
        bool checkBrackets(std::vector<std::string> line);
        bool checkFileName(std::string file);
        void validConfigFile(std::vector<std::string> line);
        bool isEmpty(std::ifstream& file);
        void analyzeConfig(std::string arg);

        // getters
        std::vector<std::string> getLines(void);
        int                      getNumServers(void);

        class exception : public std::exception {
            private:
                std::string msg;

            public:
                exception(const std::string& msg);
                virtual ~exception() throw();
                virtual const char* what() const throw();
        };
};

void        debugMode(const std::string& msg);
std::string binToHex(const char* digits);  // talvez seja util no futuro

#endif