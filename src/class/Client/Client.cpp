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

    switch (getMethodIndex(_request->getMethod())) {
        case GET:
            return runGetMethod();
        case POST:
            return runPostMethod();
        case DELETE:
            return runDeleteMethod();
        default:
            return (_request->setStatusCode(NOT_FOUND));
    }
}

int Client::runDeleteMethod()
{
    string uri = _request->getURI();
    Route matchedRoute = _server->findMatchingRoute(uri, Client::_subdirAutoindex);
    string filePath = defineFilePath(matchedRoute, uri);

    if (!Utils::fileExists(filePath)) {
        setResponseData(NOT_FOUND, "", "text/plain", "404 Not Found", "");
        return NOT_FOUND;
    }

    if (!Utils::hasDeletePermission(filePath)) {
        setResponseData(FORBIDDEN, "", "text/plain", "403 Forbidden", "");
        return FORBIDDEN;
    }

    if (remove(filePath.c_str()) != 0) {
        setResponseData(INTERNAL_SERVER_ERROR, "", "text/plain", "500 Internal Server Error", "");
        return INTERNAL_SERVER_ERROR;
    }

    setResponseData(OK, filePath, "text/plain", "200 OK", "");
    return OK;
}

int Client::runGetMethod()
{
    string uri = _request->getURI();
    Route matchedRoute = _server->findMatchingRoute(uri, _subdirAutoindex);

    if (matchedRoute.getRedirect() != ""){
        setResponseData(MOVED_PERMANENTLY, "", "", "Moved Permanently", matchedRoute.getRedirect());
        return (MOVED_PERMANENTLY);
    }

    string filePath = defineFilePath(matchedRoute, uri);
    string contentType = defineContentType(filePath);
    string responseBody = defineResponseBody(matchedRoute, filePath, uri);
    string contentLength = defineContentLength(responseBody);

    if (_response->getStatusCode() == NOT_FOUND){
        setResponseData(NOT_FOUND, filePath, "text/plain", "404 Not Found", "");
        return NOT_FOUND;
    } else if (_response->getStatusCode() == FORBIDDEN){
        setResponseData(FORBIDDEN, filePath, "text/plain", "403 Forbidden", "");
        return FORBIDDEN;
    } else if (_response->getStatusCode() == INTERNAL_SERVER_ERROR){
        setResponseData(INTERNAL_SERVER_ERROR, filePath, "text/plain", "500 Internal Server Error", "");
        return INTERNAL_SERVER_ERROR;
    }
    setResponseData(OK, filePath, contentType, responseBody, "");
    return (OK);
}

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

void Client::sendResponse(void)
{
    string build;

    if (_request->getStatusCode() == BAD_REQUEST) {
        build = _response->buildMessage();
    }
    else {
        callMethod();
        build = _response->buildMessage();
    }
    send(_server->getPollFd().fd, build.c_str(), build.size(), 0);
    cout << "Message sent" << endl;

    close(_server->getPollFd().fd);
    _request->clear();
    _response->clear();
}

string Client::defineResponseBody(const Route &route, const string& filePath, const string& uri)
{
    if (route.getCgiOn()) {
        if (filePath.find(".py") != string::npos || filePath.find(".php") != string::npos)
            return (_response->executeCGI(*_request, *_server, filePath));
    }

    struct stat path_stat;
    stat(filePath.c_str(), &path_stat);
    if (S_ISDIR(path_stat.st_mode)){
        if (_subdirAutoindex)
        	return (_response->handleAutoIndex(filePath, uri));
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

void Client::setResponseData(int statusCode, string filePath, string contentType, string responseBody, string location){
    if (statusCode == MOVED_PERMANENTLY)
        _response->setLocation(location);
    _response->setStatusCode(statusCode);
    _response->setFilePath(filePath);
    _response->setContentType(contentType);
    _response->setResponseBody(responseBody);
    _response->setContentLength(defineContentLength(responseBody));
}