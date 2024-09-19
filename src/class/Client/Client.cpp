#include "Client.hpp"

bool Client::_subdirAutoindex = false;

Client::Client()
{
}

Client::~Client()
{
}

Client::ClientException::ClientException(const string& message) : msg(message) {}
Client::ClientException::~ClientException() throw() {}

const char* Client::ClientException::what() const throw()
{
    return (msg.c_str());
}

void Client::addAssociation(int clientFd, Server& server)
{
    _fdsMap[clientFd] = &server;
}

Server* Client::getServerFd(int clientFd)
{
    Server *serverPtr = NULL;
    if (_fdsMap.find(clientFd) != _fdsMap.end())
        return (_fdsMap[clientFd]);
    else {
        cerr << RED << "Error: serverFd not found for request" << RESET;
    }
    return (serverPtr);
}

int Client::getMethodIndex(Route &route, string method)
{
    string allowedMethod;

    stringstream ss(route.getAllowMethods());

    while (ss >> allowedMethod){
        if ((allowedMethod == "get" || allowedMethod == "GET") && method == "GET")
            return (0);
        else if ((allowedMethod == "post" || allowedMethod == "POST") && method == "POST")
            return (1);
        else if ((allowedMethod == "delete" || allowedMethod == "DELETE") && method == "DELETE")
            return (2);
    }	
    return (-1);
}

int Client::callMethod()
{
    if (_request.getURI().empty()){
        setPageError(BAD_REQUEST, ERROR400);
        return (_request.setStatusCode(BAD_REQUEST));
    }

    Route matchedRoute = findMatchingRoute(_request.getURI(), _subdirAutoindex);
    string filePath = defineFilePath(matchedRoute, _request.getURI());

    switch (getMethodIndex(matchedRoute, _request.getMethod())) {
        case GET:
            return runGetMethod(filePath, matchedRoute);
        case POST:
            return runPostMethod(filePath);
        case DELETE:
            return runDeleteMethod(filePath);
        default:
            return (METHOD_NOT_ALLOWED);
    }
}

int Client::runDeleteMethod(string filePath)
{
    if (!Utils::fileExists(filePath)) {
        setResponseData(NOT_FOUND, ERROR404, "text/html", _response.getStatusPage(NOT_FOUND), "");
        return NOT_FOUND;
    }

    if (!Utils::hasDeletePermission(filePath)) {
        setResponseData(FORBIDDEN, ERROR403, "text/html", _response.getStatusPage(FORBIDDEN), "");
        return FORBIDDEN;
    }

    if (remove(filePath.c_str()) != 0) {
        setResponseData(INTERNAL_SERVER_ERROR, ERROR500, "text/html", _response.getStatusPage(INTERNAL_SERVER_ERROR), "");
        return INTERNAL_SERVER_ERROR;
    }

    setResponseData(NO_CONTENT, filePath, "text/html", _response.getStatusPage(NO_CONTENT), "");
    return NO_CONTENT;
}

Server Client::getServer()
{
    return (_server);
}

//TODO: tratar situaçao http://localhost:8080/index
Route Client::findMatchingRoute(string uri, bool& subdirAutoindex)
{
    Route routeDefault;
    string uriPath;

    if(uri == "/" || (count(uri.begin(), uri.end(), '/') == 1 && uri[0] == '/'))
        uriPath = "/";
    else
        uriPath = uri.substr(0, uri.substr(1).find("/") + 1);

    for (size_t i = 0; i < getServer().getRoute().size(); i++) {
        if (uriPath == getServer().getRoute()[i].getRoute()) {
            if (getServer().getRoute()[i].getAutoIndex())
                subdirAutoindex = true;
            else
                subdirAutoindex = false;
            routeDefault = getServer().getRoute()[i];
            return (routeDefault);
        }
    }
    return (routeDefault);
}

int Client::runGetMethod(string filePath, Route matchedRoute)
{
    if (matchedRoute.getRedirect() != "") {
        setResponseData(MOVED_PERMANENTLY, "", "text/html", _response.getStatusPage(MOVED_PERMANENTLY), matchedRoute.getRedirect());
        return (MOVED_PERMANENTLY);
    }

    if (!Utils::fileExists(filePath)) {
        setResponseData(NOT_FOUND, "", "text/html", _response.getStatusPage(NOT_FOUND), "");
        return NOT_FOUND;
    }
    string contentType = defineContentType(filePath);
    string responseBody = defineResponseBody(matchedRoute, filePath);
    string contentLength = defineContentLength(responseBody);

    if (_response.getStatusCode() == NOT_FOUND){
        setResponseData(NOT_FOUND, ERROR404, "text/html", _response.getStatusPage(NOT_FOUND), "");
        return NOT_FOUND;
    } else if (_response.getStatusCode() == FORBIDDEN){
        setResponseData(FORBIDDEN, ERROR403, "text/html", _response.getStatusPage(FORBIDDEN), "");
        return FORBIDDEN;
    } else if (_response.getStatusCode() == INTERNAL_SERVER_ERROR){
        setResponseData(INTERNAL_SERVER_ERROR, ERROR500, "text/html", _response.getStatusPage(INTERNAL_SERVER_ERROR), "");
        return INTERNAL_SERVER_ERROR;
    }
    setResponseData(OK, filePath, contentType, responseBody, "");
    return (OK);
}

void Client::sendResponse(struct pollfd& pollFds, map<int, Request>& requests)
{
    _request = requests[pollFds.fd];
    _server = _request.getServer();
    string build;

    setResponseData(_request.getStatusCode(), "", "text/html", _response.getStatusPage(_request.getStatusCode()), "");
    if (_request.getStatusCode() == DEFAULT || _request.getStatusCode() == OK){
        if (callMethod() == METHOD_NOT_ALLOWED){
            setPageError(METHOD_NOT_ALLOWED, ERROR405);
        }
    }
    build = _response.buildMessage();
    send(pollFds.fd, build.c_str(), build.size(), 0);
    cout << YELLOW << "Message sent to port: " <<  BBLUE << _server.getListen() <<  RESET << endl;
  
    requests.erase(pollFds.fd);
    close(pollFds.fd);


    _request.clear();
    _response.clear();
}

bool Client::verifyPermission(const string& file)
{
    if (access(file.c_str(), F_OK) != 0)
        return (false);
    else if (access(file.c_str(), F_OK) != 0)
        return (false);
    return (true);
}

void Client::setResponseData(int statusCode, string filePath, string contentType, string responseBody, string location)
{
    if (statusCode == MOVED_PERMANENTLY)
        _response.setLocation(location);
    _response.setStatusCode(statusCode);
    _response.setFilePath(filePath);
    _response.setContentType(contentType);
    _response.setResponseBody(responseBody);
    _response.setContentLength(defineContentLength(responseBody));
}

string Client::setPageError(int errorCode, const string& filePath){
    string errorContent = Utils::readFile(filePath);

    switch (errorCode) {
            case MOVED_PERMANENTLY:
                setResponseData(MOVED_PERMANENTLY, ERROR301, "text/html", errorContent, "");
                break ;
            case BAD_REQUEST:
                setResponseData(BAD_REQUEST, ERROR400, "text/html", errorContent, "");
                break ;
            case FORBIDDEN:
                setResponseData(FORBIDDEN, ERROR403, "text/html", errorContent, "");
                break ;
            case NOT_FOUND:
                setResponseData(NOT_FOUND, ERROR404, "text/html", errorContent, "");
                break ;
            case METHOD_NOT_ALLOWED:
                setResponseData(METHOD_NOT_ALLOWED, ERROR405, "text/html", errorContent, "");
                break ;
            case INTERNAL_SERVER_ERROR:
                setResponseData(INTERNAL_SERVER_ERROR, ERROR500, "text/html", errorContent, "");
                break ;
            case BAD_GATEWAY:
                setResponseData(BAD_GATEWAY, ERROR502, "text/html", errorContent, "");
                break ;
            default:
                setResponseData(BAD_REQUEST, ERRORUNKNOWN, "text/html", errorContent, "");
                break ;
        }
    return (errorContent);
}