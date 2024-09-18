#include "Route.hpp"

string Route::getRoute() const
{
	if (_route.empty())
    	return "";
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
    if (_root.empty())
        return ("");
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
	if (_cgi.empty())
		return ("");
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

void Route::setCgiOn(string& cgiOn)
{
    if(cgiOn == "on")
        _cgiOn = true;
}

void Route::setRoot(string& root)
{
    if (root.empty() || root[0] != '/')
        throw Route::exception(RED "Error: misformatted root path, please use '/path'" RESET);

    if (root.length() > 1 && root[root.length() - 1] == '/')
        root.erase(root.end() - 1);

    _root = "content" + root;
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

void Route::setIndex(string& index){
    if (index[0] == '/')
        throw Route::exception(RED "Error: misformatted root index, please use 'file.ext'" RESET);
    if (index[index.length() - 1] == '/')
        throw Route::exception(RED "Error: misformatted root index, please use 'file.ext'" RESET);
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
