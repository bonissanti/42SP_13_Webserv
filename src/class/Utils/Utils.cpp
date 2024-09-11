#include "Utils.hpp"

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

string Utils::itostr(int value)
{
	string result;
	stringstream ss;
	
	ss << value;
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

bool Utils::isFile(const string& path){
    struct stat info;

    if (stat(path.c_str(), &info) != 0)
        return (false);
    return (info.st_mode & S_IFREG) != 0;
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

bool Utils::uriAlreadyPresent(const string& route, const string& uri){
    size_t pos = route.find_last_of('/');
    if (pos == string::npos)
        return (false);
    if (route.substr(0, pos) == uri)
        return (true);
    return (false);
}

string Utils::removeSlash(string str){
    if (str[0] == '/')
        return (str.erase(0, 1));
    return (str);
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
        case CREATED:
            return "201 Resource Created";
        case NO_CONTENT:
            return "204 Content Deleted";
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
        case NOT_IMPLEMENTED:
            return "501 Method Not Implemented";
        case BAD_GATEWAY:
            return "502 Bad Gateway";
        case VERSION_NOT_SUPPORTED:
            return "505 HTTP Version Not Supported";
        default:
            return "Unknown Status Code";
    }
}
