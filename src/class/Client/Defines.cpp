#include "Client.hpp"

string Client::defineFilePath(Route &route, string uri){
    string filePath;
    string root = route.getRoot();
    string index = route.getIndex();

    if (route.getCgiOn()){
            filePath = root + uri.substr(route.getRoute().length()); 
        if (uri == route.getRoute())
            filePath = root + uri + "/" + index;
    }
    else if (route.getAutoIndex()){
        if (!Utils::uriAlreadyPresent(root, uri))
            filePath = root + (root[root.length() - 1] == '/' ? "" : "/");
        else
           filePath = root + uri + (uri[uri.length() - 1] == '/' ? "" : "/");
    }
    else{
        if (uri == route.getRoute() || uri == "/")
            filePath = root + "/" + index;
        else
            filePath = root + uri.substr(route.getRoute().length()); 
    }
    return (Utils::removeSlash(filePath));
}

string Client::defineContentType(string filePath)
{
    size_t index;
    string extension;
    _mimeTypes[".html"] = "text/html";
    _mimeTypes[".css"] = "text/css";
    _mimeTypes[".txt"] = "text/plain";
    _mimeTypes[".png"] = "image/png";
    _mimeTypes[".jpg"] = "image/jpg";
    _mimeTypes[".gif"] = "image/gif";
    _mimeTypes[".js"] = "application/js";
    _mimeTypes[".pdf"] = "application/pdf";

    index = filePath.rfind('.');
    if (index != string::npos) {
        extension = filePath.substr(index);
        map<string, string>::iterator it = _mimeTypes.begin();

        for (; it != _mimeTypes.end(); ++it)
            if (it->first == extension)
                return (it->second + ";charset=UTF-8");
    }
    return ("text/html;charset=UTF-8");
}

string Client::defineResponseBody(const Route& route, const string& filePath)
{
    if (route.getCgiOn()) {
        if (filePath.find(".py") != string::npos || filePath.find(".php") != string::npos)
            return (executeCGI(_request, _server, filePath));
        else if (route.getIndex().find(".php") != string::npos || route.getIndex().find(".py") != string::npos) {
            size_t found = 0;
            if (route.getIndex().find(".php") != string::npos)
                found = route.getIndex().find(".php");
            else if (route.getIndex().find(".py") != string::npos)
                found = route.getIndex().find(".py");

            if (found == 0)
                return (executeCGI(_request, _server, "/cgi/index.php"));
            size_t start = route.getIndex().rfind(' ', found);
            string file = route.getIndex().substr(start + 1, found - start - 1);
            return (executeCGI(_request, _server, file));
        }
    }

    struct stat path_stat;
    stat(filePath.c_str(), &path_stat);
    if (S_ISDIR(path_stat.st_mode)) {
        if (_subdirAutoindex)
            return (_response.handleAutoIndex(filePath, _request.getURI()));
    }

    ifstream file(filePath.c_str());
    if (!file.is_open()) {
        _response.setStatusCode(NOT_FOUND);
        return ("");
    }
    else if (file.fail()) {
        _response.setStatusCode(INTERNAL_SERVER_ERROR);
        return ("");
    }
    stringstream buffer;
    buffer << file.rdbuf();

    if (!verifyPermission(filePath)) {
        _response.setStatusCode(FORBIDDEN);
        return ("");
    }
    file.close();
    return (buffer.str());
}

// string Client::defineResponseBody(const Route &route, const string& filePath, const string& uri)
// {
//     if (route.getCgiOn()) {
//         if (filePath.find(".py") != string::npos || filePath.find(".php") != string::npos)
//             return (executeCGI(_request, _server, filePath));
//     }

//     struct stat path_stat;
//     stat(filePath.c_str(), &path_stat);
//     if (S_ISDIR(path_stat.st_mode)){
//         if (_subdirAutoindex)
//         	return (_response.handleAutoIndex(filePath, uri));
//     } 
    
//     ifstream file(filePath.c_str());
//     if (!file.is_open()) {
//         _response.setStatusCode(NOT_FOUND);
//         return ("");
//     }
//     else if (file.fail()) {
//         _response.setStatusCode(INTERNAL_SERVER_ERROR);
//         return ("");
//     }
//     stringstream buffer;
//     buffer << file.rdbuf();

//     if (!verifyPermission(filePath)) {
//         _response.setStatusCode(FORBIDDEN);
//         return ("");
//     }
//     file.close();
//     return (buffer.str());
// }

string Client::defineContentLength(const string& body)
{
    ostringstream oss;

    size_t len = body.size();
    oss << len;
    return (oss.str());
}