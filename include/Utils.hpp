#ifndef UTILS_HPP
#define UTILS_HPP

#include "defines.hpp"

class Utils
{
	public:
		static std::string trim(std::string str);
		static int strtoi(std::string number);
		static int getServersNumber(std::string filePath);
};

#endif