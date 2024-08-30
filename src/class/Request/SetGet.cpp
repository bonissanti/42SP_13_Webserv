#include "../../../include/defines.hpp"
#include "../Server/Server.hpp"
#include "Request.hpp"

string Request::getMethod() const
{
    return _method;
}

string Request::getURI() const
{
    return _uri;
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

Server Request::getServer() const
{
    return _server;
}

int Request::setStatusCode(HttpStatus code)
{
    return _statusCode = code;
}

string Request::getHeader(const string &field) const {
    map<string, string>::const_iterator it = _headers.find(field);
    if (it != _headers.end()) {
        return it->second;
    }
    return "";
}