#include "Client.hpp"

string Client::defineFilePath(Route& route, string uri)
{
    string index = route.getIndex();
    string root = route.getRoot();
    if (uri == "/") {
        return (root.substr(0, root.length()) + '/' + index);
    }
    else if (count(uri.begin(), uri.end(), '/') == 1) {
        if (uri.find('.') != string::npos) {
            if (Utils::fileExists(root + '/' + uri))
                return root.substr(0, root.length()) + '/' + uri;
            else
                return (root.substr(0, root.length()) + '/' + index);
        }
        else
            return (root.substr(0, root.length()) + '/' + index);
    }
    else if (count(uri.begin(), uri.end(), '/') > 1) {
        string file = uri.substr(uri.find_last_of("/") + 1);
        if (Utils::fileExists(root + '/' + file))
            return root.substr(0, root.length()) + '/' + file;
        else
            return (root.substr(0, root.length()) + '/' + index);
    }
    return ("NF");
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

string Client::defineContentLength(const string& body)
{
    ostringstream oss;

    size_t len = body.size();
    oss << len;
    return (oss.str());
}