#include "../include/Utils.hpp"
#include "../include/Server.hpp"

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
    if(Utils::validateFile(filePath) == false)
        throw Server::exception("Error: invalid file format");

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
        throw Server::exception(RED "Error: invalid config file" RESET);
    return serverCount;
}

char* Utils::strdup(const string str)
{
	size_t i = 0;
	char *dest;

	dest = new char[str.size() + 1];
	for (; i < str.size(); i++)
		dest[i] = str[i];
	dest[i] = '\0';
	return (dest);
}
