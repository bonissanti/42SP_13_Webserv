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
    Route routes = findMatchingRoute(_request.getURI(), _subdirAutoindex);
    if (_request.getURI().empty())
        setPageError(BAD_REQUEST, ERROR400);
        return (_request.setStatusCode(BAD_REQUEST));

    switch (getMethodIndex(routes, _request.getMethod())) {
        case GET:
            return runGetMethod();
        case POST:
            return runPostMethod();
        case DELETE:
            return runDeleteMethod();
        default:
            return (METHOD_NOT_ALLOWED);
    }
}

int Client::runDeleteMethod()
{
    string uri = _request.getURI();
    Route matchedRoute = findMatchingRoute(uri, Client::_subdirAutoindex);
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
    string uri = _request.getURI();
    Route matchedRoute = findMatchingRoute(uri, _subdirAutoindex);

    if (matchedRoute.getRedirect() != ""){
        setResponseData(MOVED_PERMANENTLY, "", "", "Moved Permanently", matchedRoute.getRedirect());
        return (MOVED_PERMANENTLY);
    }

    string filePath = defineFilePath(matchedRoute, uri);
    string contentType = defineContentType(filePath);
    string responseBody = defineResponseBody(matchedRoute, filePath, uri);
    string contentLength = defineContentLength(responseBody);

    if (_response.getStatusCode() == NOT_FOUND){
        setPageError(NOT_FOUND, ERROR404);
        return NOT_FOUND;
    } else if (_response.getStatusCode() == FORBIDDEN){
        setPageError(FORBIDDEN, ERROR403);
        return FORBIDDEN;
    } else if (_response.getStatusCode() == INTERNAL_SERVER_ERROR){
        setPageError(INTERNAL_SERVER_ERROR, ERROR500);
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

    if (_request.getStatusCode() == BAD_REQUEST) {
        build = _response.buildMessage();
    } else {
        if (callMethod() == METHOD_NOT_ALLOWED){
            setPageError(METHOD_NOT_ALLOWED, ERROR405);
        }
        build = _response.buildMessage();
    }
    send(pollFds.fd, build.c_str(), build.size(), 0);
    cout << "Message sent" << endl;

    requests.erase(pollFds.fd);
    close(pollFds.fd);

    _request.clear();
    _response.clear();
}

Route Client::findMatchingRoute(string uri, bool& subdirAutoindex){
    Route routeDefault;

    for (size_t i = 0; i < _server.getRoute().size(); i++){
        if (uri == _server.getRoute()[i].getRoute()){
            if (_server.getRoute()[i].getAutoIndex())
                subdirAutoindex = true;
            else
                subdirAutoindex = false;
            routeDefault = _server.getRoute()[i];
            return (routeDefault);
        }
    }

    if (uri.find("/cgi") == 0){
        routeDefault.setCgiOn(true);
    }
    return (routeDefault);
}

bool Client::verifyPermission(const string& file)
{
    if (access(file.c_str(), F_OK) != 0)
        return (false);
    else if (access(file.c_str(), F_OK) != 0)
        return (false);
    return (true);
}

void Client::setResponseData(int statusCode, string filePath, string contentType, string responseBody, string location){
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