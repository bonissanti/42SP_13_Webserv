#include "../include/Validate.hpp"

#include "../include/Utils.hpp"

Validate::Validate(string arg) : _numRoute(0)
{
    debugMode("<Validate> Default Constructor called");

    if (checkFileName(arg) == 0)
        throw Validate::exception(RED "Error: incorrect config file" RESET);
    analyzeConfig(arg);
}

vector<string> Validate::getLines(void)
{
    return (this->_lines);
}

// exception
Validate::exception::exception(const string& msg) : msg(msg) {};
Validate::exception::~exception() throw() {};
const char* Validate::exception::what() const throw()
{
    return (msg.c_str());
}

bool Validate::checkBrackets(vector<string> line)
{
    int openBrackets = 0;
    int closeBrackets = 0;

    for (vector<string>::iterator vecIt = line.begin(); vecIt != line.end(); ++vecIt) {
        for (string::iterator it = vecIt->begin(); it != vecIt->end(); ++it) {
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

void Validate::validConfigFile(vector<string> line)
{
    int nestedServer = 0;
    bool minimumReq;

    for (size_t i = 0; i < line.size(); i++) {
        if (line[i].empty())
            continue;
        if (line[i].substr(0, 6) == "server") {
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
        throw Validate::exception(RED "Error: port number (listen <number>) not informed in config file" RESET);
    cout << _numRoute << endl;
}

bool Validate::checkFileName(string file)
{
    string tmp = file.substr(file.find(".") + 1);
    if (file.substr(file.find(".") + 1) == "conf")
        return (true);
    return (false);
}

bool Validate::isEmpty(ifstream& file)
{
    return (file.peek() == ifstream::traits_type::eof());
}

void Validate::analyzeConfig(string arg)
{
    string temp;
    ifstream file(arg.c_str());

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