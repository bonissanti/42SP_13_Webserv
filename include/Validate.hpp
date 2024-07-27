#ifndef VALIDATE_HPP
#define VALIDATE_HPP

#include "Utils.hpp"
#include "defines.hpp"

/* Classe de parser, basicamente ela valida o arquivo de configuração e popula
um vector com as linhas retiradas do arquivo de configuração. É este vector que
a classe ServerConfig utiliza para configurar o servidor */

class Validate {
    private:
        int _numRoute;
        vector<string> _lines;

    public:
        Validate(string arg);
        ~Validate();

        // validations
        bool checkBrackets(vector<string> line);
        bool checkFileName(string file);
        void validConfigFile(vector<string> line);
        bool isEmpty(ifstream& file);
        void analyzeConfig(string arg);

        // getters
        vector<string> getLines(void);
        int getNumServers(void);

        class exception : public std::exception {
            private:
                string msg;

            public:
                exception(const string& msg);
                virtual ~exception() throw();
                virtual const char* what() const throw();
        };
};

void debugMode(const string& msg);
string binToHex(const char* digits);  // talvez seja util no futuro

#endif