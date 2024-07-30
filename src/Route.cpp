#include "../include/Route.hpp"

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

void Route::create(const string& line, ifstream& file)
{
    string currentLine = line;
    string key, value;
    size_t pos;

    currentLine = Utils::trim(currentLine);

    pos = currentLine.find(' ');
    if (pos != string::npos and currentLine.find("redirect{") == string::npos) {
        int pos_col = currentLine.find('{') -1;
        _route = Utils::trim(currentLine.substr(6, pos_col - 6));
    }
    else if (pos != string::npos) {
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
            key = currentLine.substr(0, pos);
            value = currentLine.substr(pos + 1);

            key = Utils::trim(key);
            value = Utils::trim(value);

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

string Route::getRoute() const
{
    return _route;
}

bool Route::getAutoIndex() const
{
    return _autoIndex;
}

bool Route::getCgiOn() const
{
    return _cgiOn;
}

string Route::getRoot() const
{
    return _root;
}

string Route::getAllowMethods() const
{
    return _allowMethods;
}

string Route::getIndex() const
{
    return _index;
}

string Route::getRedirect() const
{
    return _redirect;
}

string Route::getReturn() const
{
    return _return;
}

string Route::getCgi() const
{
    return _cgi;
}

void Route::setRoute(const string& route)
{
    _route = route;
}

void Route::setAutoIndex(bool autoIndex)
{
    _autoIndex = autoIndex;
}

void Route::setCgiOn(bool cgiOn)
{
    _cgiOn = cgiOn;
}

void Route::setRoot(string& root)
{
    if (root[0] != '/')
        throw Route::exception(RED "Error: misformatted root path, please use '/path'" RESET);
    if (root.substr(root.length() - 1) != "/")
        root.insert(root.end(), '/');
    _root = root;
}

void Route::setAllowMethods(const string& allowMethods)
{
    string allow_method;
    stringstream ss(allowMethods);

    while (ss >> allow_method) {
        if (allow_method == "get" || allow_method == "GET")
            continue;

        else if (allow_method == "post" || allow_method == "POST")
            continue;

        else if (allow_method == "delete" || allow_method == "DELETE")
            continue;
        else
            throw invalid_argument(RED "Error: invalid method" RESET);
    }
    _allowMethods = allowMethods;
}

void Route::setIndex(string& index)
{
    if (index[0] == '/')
        index.erase(index.begin());
    if (index.substr(index.length() - 1) == "/")
        index.erase(index.length() - 1);
}

void Route::setRedirect(const string& redirect)
{
    if (redirect[0] == '/' || redirect.empty())
        throw Route::exception(RED "Error: this is not a valid url" RESET);
    _redirect = redirect;
}

void Route::setReturn(const string& ret)
{
    _return = ret;
}

void Route::setCgi(string& cgi)
{
    if (cgi.substr(cgi.find_last_of(".") + 1) != ".py")
        throw invalid_argument(RED "Error: file extension is not '.py'" RESET);

    if (cgi[0] == '/')
        cgi.erase(cgi.begin());
    if (cgi.substr(cgi.length() - 1) == "/")
        cgi.erase(cgi.length() - 1);
    _cgi = cgi;
}

Route::exception::exception(const std::string& msg) : msg(msg) {}

Route::exception::~exception() throw() {}

const char* Route::exception::what() const throw()
{
    return msg.c_str();
}