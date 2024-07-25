#include "../include/Utils.hpp"
#include <stack>

std::string Utils::trim(std::string str)
{
    // trim right
    size_t pos = str.find_last_not_of(" \t\n");
    str.erase(pos + 1);

    // trim left
    pos = str.find_first_not_of(" \t\n");
    str.erase(0, pos);
    return (str);
}

int Utils::strtoi(std::string number)
{
    int result;
    std::stringstream ss;

    ss << number;
    ss >> result;
    return (result);
}

int Utils::getServersNumber(std::string filePath)
{
    std::ifstream file(filePath.c_str());
    if (!file.is_open()) {
        return -1;
    }

    std::string line;
    int serverCount = 0;
    bool insideServerBlock = false;
    std::stack<char> brackets;

    while (std::getline(file, line)) {
        for (std::string::size_type i = 0; i < line.size(); ++i) {
            char c = line[i];

            if (!insideServerBlock && line.substr(i, 6) == "server") {
                insideServerBlock = true;
                i += 5;
                continue;
            }

            if (insideServerBlock) {
                if (c == '{') {
                    brackets.push(c);
                }
                else if (c == '}') {
                    if (brackets.empty()) {
                        return -1;
                    }
                    brackets.pop();
                    if (brackets.empty()) {
                        insideServerBlock = false;
                        ++serverCount;
                    }
                }
            }
        }
    }

    if (!brackets.empty()) {
        return -1;
    }

    return serverCount;
}