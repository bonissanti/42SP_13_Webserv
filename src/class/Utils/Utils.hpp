#ifndef UTILS_HPP
#define UTILS_HPP


typedef enum {
    DEFAULT = 0,
    OK = 200,
    CREATED = 201,
    ACCEPTED = 202,
    NO_CONTENT = 204,
    MOVED_PERMANENTLY = 301,
    NOT_MODIFIED = 304,
    BAD_REQUEST = 400,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    METHOD_NOT_ALLOWED = 405,
    CONFLICT = 409,
    INTERNAL_SERVER_ERROR = 500,
    NOT_IMPLEMENTED = 501,
    BAD_GATEWAY = 502,
    VERSION_NOT_SUPPORTED = 505,
} 	HttpStatus;

#define RED "\033[0;31m"
#define BYELLOW "\033[1;33m"
#define YELLOW "\033[0;33m"
#define PURPLE "\033[0;34m"
#define BBLUE "\033[1;34m"
#define BWHITE "\033[1;37m"
#define BLACK "\033[1;30m"
#define GREEN "\033[0;32m"
#define RESET "\033[0m"

// Macro configs
#define LISTEN 0
#define SERVER_NAME 1
#define HOST 2
#define ROOT 3
#define CLIENT_MBSIZE 4
#define ROUTE 5

// Macro route
#define CGIDIR "/cgi/"
#define CGIEXEC "/usr/bin/python3"
#define AUTOINDEX 0
#define RROOT 1
#define AMETHODS 2
#define INDEX 3
#define CGI 4
#define REDIRECT 5

// Methods
#define GET 0
#define POST 1
#define DELETE 2

#define MB 1024
#define GB 1048576

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <dirent.h>

#include <cstring>
#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <cstddef>
#include <sys/stat.h>

using namespace std;
extern bool signalUsed;

class Utils {
    public:
        static string trim(string str);
        static int strtoi(string number);
        static int getServersNumber(string filePath);
        static bool validateFile(string file);
        static void bzero(void* ptr, size_t size);
        static string toString(size_t value);
        static bool fileExists(const std::string& filePath);
        static bool hasDeletePermission(const std::string& filePath);
        static bool isFile(const string& path);
        static bool uriAlreadyPresent(const string& route, const string& uri);
        static string itostr(int value);
        static string statusCodeToString(int code);
        static string removeSlash(string str);
};

#endif