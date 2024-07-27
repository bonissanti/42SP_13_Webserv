#ifndef UTILS_HPP
#define UTILS_HPP

#include "defines.hpp"

class Utils {
    public:
        static string trim(string str);
        static int strtoi(string number);
        static int getServersNumber(string filePath);
};

#endif