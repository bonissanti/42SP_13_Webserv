#include "Client.hpp"

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

Server Client::getServer(void)
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

void Client::setServer(Server server)
{
    _server = server;
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
    Route matchedRoute = _server.findMatchingRoute(uri);
    string filePath = defineFilePath(matchedRoute, uri);

    if (!Utils::fileExists(filePath)) {
        _request->setStatusCode(NOT_FOUND);
        return NOT_FOUND;
    }

    if (!Utils::hasDeletePermission(filePath)) {
        _request->setStatusCode(FORBIDDEN);
        return FORBIDDEN;
    }

    if (remove(filePath.c_str()) != 0) {
        _request->setStatusCode(INTERNAL_SERVER_ERROR);
        return INTERNAL_SERVER_ERROR;
    }

    _response->setStatusCode(OK);
    return OK;
}

int Client::runGetMethod()
{
    string uri = _request->getURI();
    Route matchedRoute = _server.findMatchingRoute(uri);

    string filePath = defineFilePath(matchedRoute, uri);
    string contentType = defineContentType(filePath);
    string responseBody = defineResponseBody(filePath, uri);
    string contentLength = defineContentLength(responseBody);

    _response->setFilePath(filePath);
    _response->setContentType(contentType);
    _response->setResponseBody(responseBody);
    _response->setContentLength(contentLength);
    _response->setStatusCode(OK);  // TODO: colocar o status code correto conforme o ocorrido
    return (OK);
}
    
// string Client::defineFilePath(Route &route, string uri)
// {   
//     /* TODO: reformular essa função, as vezes o cliente solicita algo com '/' no inicio. 
//     Especialmente quando usado autoindex, esse é um ponto de atenção */

//     string filePath;

//     if (uri == "/") {
//         filePath = defineHome(_server.getRoute());
//     }
//     else if (uri == "/cgi"){
//     	filePath = "content" + uri + "/" + _server.getRoute()[0].getIndex();
//     }
//     else {
//         filePath = "content" + _request->getURI();  // TODO: nem sempre a pasta sera a content, precisa ler e pegar
//                                                     // corretamente a pasta conforme a rota
//     }
//     return (filePath);
// }

string Client::defineFilePath(Route &route, string uri){
    string filePath;

    if (uri == route.getRoute() || uri == "/")
        filePath = route.getRoot() + route.getIndex();
    else
        filePath = route.getRoot() + uri; 
    return (filePath);
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
    send(_server.getPollFd().fd, build.c_str(), build.size(), 0);
    cout << "Message sent" << endl;

    close(_server.getPollFd().fd);
    _request->clear();
    _response->clear();
}

string Client::defineResponseBody(const string& filePath, const string& uri)
{
    if (_request->getIsCgi()) {
        _response->getIndex() = _server.getRoute()[0].getIndex();
        cerr << _response->getIndex() << endl;

        if (_response->getIndex().find(".py") != string::npos || _response->getIndex().find(".php") != string::npos)
            return (_response->executeCGI(*_request, _server, uri));
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