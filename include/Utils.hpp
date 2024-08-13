#ifndef UTILS_HPP
#define UTILS_HPP

#include "defines.hpp"

class Utils {
    public:
        static string trim(string str);
        static int strtoi(string number);
        static int getServersNumber(string filePath);
        static bool validateFile(string file);
        static void bzero(void *ptr, size_t size);

};

string to_string(double value);
string to_string(int value);

#endif