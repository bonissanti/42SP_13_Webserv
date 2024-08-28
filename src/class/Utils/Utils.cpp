#include "../include/Utils.hpp"
#include "../include/defines.hpp"
#include "class/Server/Server.hpp"

#include <sys/stat.h>
#include <stack>

string Utils::trim(string str)
{
    // trim right
    size_t pos = str.find_last_not_of(" \t\n\r");
    str.erase(pos + 1);

    // trim left
    pos = str.find_first_not_of(" \t\n\r");
    str.erase(0, pos);
    return (str);
}

int Utils::strtoi(string number)
{
    int result;
    stringstream ss;

    ss << number;
    ss >> result;
    return (result);
}

void    Utils::bzero(void *ptr, size_t size)
{
    size_t i = -1;
    char *str = static_cast<char *>(ptr);

    while (++i < size){
        str[i] = 0;
    }
}

bool Utils::validateFile(string file_name)
{
    if (file_name.find(".conf") == string::npos)
        return false;
    ifstream file(file_name.c_str());
    if (file.peek() == ifstream::traits_type::eof())
        return false;
    return true;
}

int Utils::getServersNumber(string filePath)
{
   
}

void Utils::debugMode(const string& msg)
{
#ifndef DEBUG
    (void)msg;
#endif

#ifdef DEBUG
    cerr << BBLUE << msg << RESET << endl;
#endif
}

// Verifica se o arquivo existe
bool Utils::fileExists(const std::string& filePath) {
    struct stat buffer;
    return (stat(filePath.c_str(), &buffer) == 0);
}

// Verifica se o servidor tem permissão para excluir o arquivo
bool Utils::hasDeletePermission(const std::string& filePath) {
    if (access(filePath.c_str(), W_OK) != 0) {
        return false;
    }
    return true;
}

string Utils::toString(size_t value) {
    ostringstream oss;
    oss << value;
    return oss.str();
}

string Utils::statusCodeToString(int code) {
    switch (code) {
        case DEFAULT:
            return "Default";
        case OK:
            return "200 OK";
        case MOVED_PERMANENTLY:
            return "301 Moved Permanently";
        case BAD_REQUEST:
            return "400 Bad Request";
        case FORBIDDEN:
            return "403 Forbidden";
        case NOT_FOUND:
            return "404 Not Found";
        case METHOD_NOT_ALLOWED:
            return "405 Method Not Allowed";
        case INTERNAL_SERVER_ERROR:
            return "500 Internal Server Error";
        case BAD_GATEWAY:
            return "502 Bad Gateway";
        default:
            return "Unknown Status Code";
    }
}