#include "../../../include/defines.hpp"
#include "../Server/Server.hpp"
#include "Request.hpp"

string Request::getMethod() const
{
    return _method;
}

string Request::getPath() const
{
    return _path;
}

string Request::getVersion() const
{
    return _version;
}

string Request::getBody() const
{
    return _body;
}

bool Request::getIsCgi() const
{
    return _isCgi;
}

int Request::getStatusCode() const
{
    return _statusCode;
}

map<int, Request> Request::getRequest() const
{
    return _requests;
}

Server Request::getServer() const
{
    return _server;
}

int Request::setStatusCode(HttpStatus code)
{
    return _statusCode = code;
}