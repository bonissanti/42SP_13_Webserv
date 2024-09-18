#include "Client.hpp"

bool Client::_subdirAutoindex = false;

Client::Client()
{
    _request = new Request();
    _response = new Response();
}

Client::~Client()
{
    delete _request;
    delete _response;
}

Client::ClientException::ClientException(const string& message) : msg(message) {}
Client::ClientException::~ClientException() throw() {}

const char* Client::ClientException::what() const throw()
{
    return (msg.c_str());
}

int Client::getMethodIndex(string method)
{
    string types[] = {"GET", "POST", "DELETE"};

    for (size_t i = 0; i < 3; i++)
        if (types[i] == method)
            return (i);
    return (-1);
}

Server* Client::getServer(void)
{
    return (_server);
}

Request* Client::getRequest(void)
{
    return (_request);
}

Response* Client::getResponse(void)
{
    return (_response);
}

void Client::setServer(Server& server)
{
    _server = &server;
}

int Client::callMethod()
{
    if (_request->getURI().empty())
        return (_request->setStatusCode(BAD_REQUEST));

    Route matchedRoute = findMatchingRoute(_request->getURI(), _subdirAutoindex);
    string filePath = defineFilePath(matchedRoute, _request->getURI());

    switch (getMethodIndex(_request->getMethod())) {
        case GET:
            return runGetMethod(filePath, matchedRoute);
        case POST:
            return runPostMethod(filePath);
        case DELETE:
            return runDeleteMethod(filePath);
        default:
            return (_request->setStatusCode(NOT_FOUND));
    }
}

int Client::runDeleteMethod(string filePath)
{
    if (!Utils::fileExists(filePath)) {
        setResponseData(NOT_FOUND, ERROR404, "text/html", _response->getStatusPage(NOT_FOUND), "");
        return NOT_FOUND;
    }

    if (!Utils::hasDeletePermission(filePath)) {
        setResponseData(FORBIDDEN, ERROR403, "text/html", _response->getStatusPage(FORBIDDEN), "");
        return FORBIDDEN;
    }

    if (remove(filePath.c_str()) != 0) {
        setResponseData(INTERNAL_SERVER_ERROR, ERROR500, "text/html", _response->getStatusPage(INTERNAL_SERVER_ERROR), "");
        return INTERNAL_SERVER_ERROR;
    }

    setResponseData(NO_CONTENT, filePath, "text/html", _response->getStatusPage(NO_CONTENT), "");
    return NO_CONTENT;
}
// TODO: Tratar http://localhost:8080/index
// TODO: Tratar situacao de diretorio ou arquivo
    
Route Client::findMatchingRoute(string uri, bool& subdirAutoindex)
{
    Route routeDefault;
    string uriPath;

    if(uri == "/")
        uriPath = "/";
    else if(std::count(uri.begin(), uri.end(), '/') == 1)
        uriPath = uri.substr(0, uri.size());
    else
        uriPath = uri.substr(0, uri.substr(1).find("/") + 1);


    for (size_t i = 0; i < getServer()->getRoute().size(); i++) {
        if (uriPath == getServer()->getRoute()[i].getRoute()) {
            if (getServer()->getRoute()[i].getAutoIndex())
                subdirAutoindex = true;
            else
                subdirAutoindex = false;
            routeDefault = getServer()->getRoute()[i];
            return (routeDefault);
        }
    }
    return (routeDefault);
}

int Client::runGetMethod(string filePath, Route matchedRoute)
{
    if (matchedRoute.getRedirect() != "") {
        setResponseData(MOVED_PERMANENTLY, "", "text/html", _response->getStatusPage(MOVED_PERMANENTLY), matchedRoute.getRedirect());
        return (MOVED_PERMANENTLY);
    }

    // Descomentar apos merge Renato = 17/09
    // if (!Utils::fileExists(filePath)) {
    //     setResponseData(NOT_FOUND, "", "text/html", _response->getStatusPage(NOT_FOUND), "");
    //     return NOT_FOUND;
    // }
    string contentType = defineContentType(filePath);
    string responseBody = defineResponseBody(matchedRoute, filePath);
    string contentLength = defineContentLength(responseBody);

    if (_response->getStatusCode() == NOT_FOUND){
        setResponseData(NOT_FOUND, ERROR404, "text/html", _response->getStatusPage(NOT_FOUND), "");
        return NOT_FOUND;
    } else if (_response->getStatusCode() == FORBIDDEN){
        setResponseData(FORBIDDEN, ERROR403, "text/html", _response->getStatusPage(FORBIDDEN), "");
        return FORBIDDEN;
    } else if (_response->getStatusCode() == INTERNAL_SERVER_ERROR){
        setResponseData(INTERNAL_SERVER_ERROR, ERROR500, "text/html", _response->getStatusPage(INTERNAL_SERVER_ERROR), "");
        return INTERNAL_SERVER_ERROR;
    }
    setResponseData(OK, filePath, contentType, responseBody, "");
    return (OK);
}



string Client::defineFilePath(Route& route, string uri)
{
    string root = route.getRoot();
    string index = route.getIndex();
    string routePath = route.getRoute();

    if (uri.find(routePath) == 0 && routePath.length() > 1)
        uri = uri.substr(routePath.length());
    if (root[root.length() - 1] != '/')
        root += "/";
    if (!uri.empty() && uri[0] == '/')
        uri = uri.substr(1);
    string filePath = root + uri;

    return (filePath);
}

// string Client::defineFilePath(Route& route, string uri)
// {
//     string root = route.getRoot();
//     string index = route.getIndex();
//     string routePath = route.getRoute();

//     if (uri.find(routePath) == 0 && routePath.length() > 1)
//         uri = uri.substr(routePath.length());
//     // if (root[root.length() - 1] != '/')
//     //     root += "/";
//     string filePath = root + uri;

//     if (filePath.length() > 1 && filePath[filePath.length() - 1] == '/')
//         filePath = filePath.substr(0, filePath.length() - 1);

//     if (route.getAutoIndex()) {
//         if (!Utils::uriAlreadyPresent(root, filePath))
//             filePath = root + (root[root.length() - 1] == '/' ? "" : "/");
//         else
//             filePath = root + uri + (uri[uri.length() - 1] == '/' ? "" : "/");
//         if (!uri.empty() && uri[0] == '/')
//             uri = uri.substr(1);
//         return (filePath);
//     }
//     if (!uri.empty() && uri[0] == '/')
//         uri = uri.substr(1);
//     return (filePath);
// }
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

void Client::sendResponse(void)
{
    string build;

    setResponseData(_request->getStatusCode(), "", "text/html", _response->getStatusPage(_request->getStatusCode()), "");
    if (_request->getStatusCode() == DEFAULT || _request->getStatusCode() == OK)
            callMethod();
    build = _response->buildMessage();
    send(_server->getPollFd().fd, build.c_str(), build.size(), 0);
    cout << "Message sent" << endl;

    close(_server->getPollFd().fd);
    _request->clear();
    _response->clear();
}

string Client::defineResponseBody(const Route& route, const string& filePath)
{
    if (route.getCgiOn()) {
        if (filePath.find(".py") != string::npos || filePath.find(".php") != string::npos)
            return (_response->executeCGI(*_request, *_server, filePath));
        else if (route.getIndex().find(".php") != string::npos || route.getIndex().find(".py") != string::npos) {
            size_t found = 0;
            if (route.getIndex().find(".php") != string::npos)
                found = route.getIndex().find(".php");
            else if (route.getIndex().find(".py") != string::npos)
                found = route.getIndex().find(".py");

            if (found == 0)
                return (_response->executeCGI(*_request, *_server, "/cgi/index.php"));
            size_t start = route.getIndex().rfind(' ', found);
            string file = route.getIndex().substr(start + 1, found - start - 1);
            return (_response->executeCGI(*_request, *_server, file));
        }
    }

    struct stat path_stat;
    stat(filePath.c_str(), &path_stat);
    if (S_ISDIR(path_stat.st_mode)) {
        if (_subdirAutoindex)
            return (_response->handleAutoIndex(filePath, _request->getURI()));
    }

    ifstream file(filePath.c_str());
    if (!file.is_open()) {
        _response->setStatusCode(NOT_FOUND);
        return ("");
    }
    else if (file.fail()) {
        _response->setStatusCode(INTERNAL_SERVER_ERROR);
        return ("");
    }
    stringstream buffer;
    buffer << file.rdbuf();

    if (!verifyPermission(filePath)) {
        _response->setStatusCode(FORBIDDEN);
        return ("");
    }
    file.close();
    return (buffer.str());
}

void Client::handleMultiPartRequest(void)
{
    char buffer[65535];
    ssize_t bytesReceived = recv(_server->getPollFd().fd, buffer, sizeof(buffer), 0);

    if (bytesReceived > 0) {
        _request->parseRequest(string(buffer, bytesReceived));
    }
    else if (bytesReceived == 0) {
        cout << "Connection closed" << endl;
        close(_server->getPollFd().fd);
    }
    else {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return;
        perror("Error: recv failed");
        close(_server->getPollFd().fd);
    }
}

bool Client::verifyPermission(const string& file)
{
    if (access(file.c_str(), F_OK) != 0)
        return (false);
    else if (access(file.c_str(), F_OK) != 0)
        return (false);
    return (true);
}

string Client::defineContentLength(const string& body)
{
    ostringstream oss;

    size_t len = body.size();
    oss << len;
    return (oss.str());
}

void Client::setResponseData(int statusCode, string filePath, string contentType, string responseBody, string location)
{
    if (statusCode == MOVED_PERMANENTLY)
        _response->setLocation(location);
    _response->setStatusCode(statusCode);
    _response->setFilePath(filePath);
    _response->setContentType(contentType);
    _response->setResponseBody(responseBody);
    _response->setContentLength(defineContentLength(responseBody));
}