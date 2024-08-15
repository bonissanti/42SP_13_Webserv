#include "Route.hpp"

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

// SETTERS

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
    _index = index;
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