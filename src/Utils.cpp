#include "../include/Utils.hpp"

#include <stack>

#include "../include/Validate.hpp"

string Utils::trim(string str)
{
    // trim right
    size_t pos = str.find_last_not_of(" \t\n");
    str.erase(pos + 1);

    // trim left
    pos = str.find_first_not_of(" \t\n");
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

int Utils::getServersNumber(string filePath)
{
    ifstream file(filePath.c_str());
    if (!file.is_open()) {
        return -1;
    }

    string line;
    int serverCount = 0;
    bool insideServerBlock = false;
    stack<char> brackets;

    while (getline(file, line)) {
        for (string::size_type i = 0; i < line.size(); ++i) {
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
                        serverCount = -1;
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
    if (serverCount == -1 && serverCount > 1024)
        throw Validate::exception(RED "Error: invalid config file" RESET);
    else if (brackets.empty())
        throw Validate::exception(RED "Error: empty config file" RESET);
    return serverCount;
}
