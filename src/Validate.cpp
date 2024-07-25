#include "../include/Validate.hpp"
#include "../include/Utils.hpp"

Validate::Validate(std::string arg) : _numServers(0), _numRoute(0)
{
    debugMode("<Validate> Default Constructor called");

    if (checkFileName(arg) == 0)
        throw Validate::exception(RED "Error: incorrect config file" RESET);
    analyzeConfig(arg);
}

std::vector<std::string> Validate::getLines(void)
{
    return (this->_lines);
}

int Validate::getNumServers(void)
{
    return (this->_numServers);
}

// exception
Validate::exception::exception(const std::string& msg) : msg(msg){};
Validate::exception::~exception() throw(){};
const char* Validate::exception::what() const throw()
{
    return (msg.c_str());
}

bool Validate::checkBrackets(std::vector<std::string> line)
{
    int openBrackets = 0;
    int closeBrackets = 0;

    for (std::vector<std::string>::iterator vecIt = line.begin(); vecIt != line.end(); ++vecIt) {
        for (std::string::iterator it = vecIt->begin(); it != vecIt->end(); ++it) {
            if (*it == '{')
                openBrackets++;
            else if (*it == '}')
                closeBrackets++;
        }
    }
    if (openBrackets > closeBrackets)
        throw Validate::exception(RED "Error: Nested braces are not allowed" RESET);
    else if (openBrackets < closeBrackets)
        throw Validate::exception(RED "Error: Closing brace without opening brance" RESET);
    return (true);
}

void Validate::validConfigFile(std::vector<std::string> line)
{
    int  nestedServer = 0;
    bool minimumReq;

    for (size_t i = 0; i < line.size(); i++) {
        if (line[i].empty())
            continue;
        if (line[i].substr(0, 6) == "server") {
            _numServers++;
            nestedServer++;
            if (nestedServer > 1)
                throw Validate::exception(RED "Error: Nested servers are not allowed" RESET);
            minimumReq = false;
        }
        if (line[i].substr(0, 5) == "route")
            _numRoute++;
        else if (line[i].substr(0, 6) == "listen")
            minimumReq = true;
        else if (line[i].substr(0, 1) == "}")
            if (nestedServer > 0)
                nestedServer--;
    }
    if (!minimumReq)
        throw Validate::exception(
            RED "Error: port number (listen <number>) not informed in config file" RESET);
    std::cout << _numRoute << std::endl;
}

bool Validate::checkFileName(std::string file)
{
    std::string tmp = file.substr(file.find(".") + 1);
    if (file.substr(file.find(".") + 1) == "conf")
        return (true);
    return (false);
}

bool Validate::isEmpty(std::ifstream& file)
{
    return (file.peek() == std::ifstream::traits_type::eof());
}

void Validate::analyzeConfig(std::string arg)
{
    std::string   temp;
    std::ifstream file(arg.c_str());

    if (isEmpty(file)) {
        file.close();
        throw Validate::exception(RED "Error: config file is empty or doesn't exist" RESET);
    }

    // analyze first line
    getline(file, temp);
    _lines.push_back(Utils::trim(temp));
    if (_lines[0].substr(0, 6) != "server") {
        file.close();
        throw Validate::exception(RED "Error: misformatted config file" RESET);
    }

    // if first line okay, start to analyze other lines
    while (getline(file, temp))
        _lines.push_back(Utils::trim(temp));

    if (!Validate::checkBrackets(_lines)) {
        file.close();
    }
    validConfigFile(_lines);
}