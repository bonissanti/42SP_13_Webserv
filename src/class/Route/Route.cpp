#include "Route.hpp"
#include <string>

// CONSTRUCTOR & DESTRUCTOR

Route::Route()
    : _route("")
    , _autoIndex(false)
    , _cgiOn(false)
    , _root("/data/")
    , _allowMethods("GET POST DELETE")
    , _index("index.html")
    , _redirect("")
    , _return("")
    , _cgi("")
{
}

Route::~Route() {}

// ROUTE FUNCTIONS

void Route::create(const string& line, ifstream& file)
{
    string currentLine = line;
    string key, value;
    size_t pos;

    currentLine = Utils::trim(currentLine);

    pos = currentLine.find(' ');
    if (pos != string::npos) {
        int pos_col = currentLine.find('{');
        _route = Utils::trim(currentLine.substr(6, pos_col - 6));
    }
    else {
        throw Route::exception("Error: Invalid route format");
    }

    while (getline(file, currentLine)) {
        currentLine = Utils::trim(currentLine);

        if (currentLine == "}")
            break;

        pos = currentLine.find('=');
        if (currentLine[0] == '#' or currentLine.empty())
            continue;
        if (pos != string::npos) {
            key = Utils::trim(currentLine.substr(0, pos));
            value = Utils::trim(currentLine.substr(pos + 1));

            if (key == "allow_methods")
                setAllowMethods(value);
            else if (key == "root")
                setRoot(value);
            else if (key == "index")
                setIndex(value);
            else if (key == "autoindex")
                setAutoIndex(value == "true");
            else if (key == "cgi")
                setCgiOn(value == "true");
            else if (key == "redirect")
                setRedirect(value);
            else if (key == "return")
                setReturn(value);
            else if (key == "cgi")
                setCgi(value);
            else
                throw Route::exception("Error: Unknown configuration key: " + key);
        }
        else
            throw Route::exception("Error: Invalid configuration line: " + currentLine);
    }
}

// EXCEPTIONS

Route::exception::exception(const string& msg) : msg(msg) {}
Route::exception::~exception() throw() {}

const char* Route::exception::what() const throw()
{
    return msg.c_str();
}