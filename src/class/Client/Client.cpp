#include "Client.hpp"

Client::Client() {}

Client::~Client() {}

Client::ClientException::ClientException(const string& message) : msg(message) {}
Client::ClientException::~ClientException() throw() {}

const char* Client::ClientException::what() const throw()
{
    return (msg.c_str());
}

void Client::addAssociation(int clientFd, Server server)
{
    _fdsMap[clientFd] = server;
}

Server Client::getServerFd(int clientFd)
{
    Server null;
    if (_fdsMap.find(clientFd) != _fdsMap.end())
        return (_fdsMap[clientFd]);
    else {
        cerr << RED << "Error: serverFd not found for request" << RESET;
    }
    return (null);
}

int Client::getMethodIndex(string method)
{
    string types[] = {"GET", "POST", "DELETE"};

    for (size_t i = 0; i < 3; i++)
        if (types[i] == method)
            return (i);
    return (-1);
}

int Client::callMethod()
{
    if (_request.getURI().empty())
        return (_request.setStatusCode(BAD_REQUEST));

    switch (getMethodIndex(_request.getMethod())) {
        case GET:
           return runGetMethod();
        case POST:
           return runPostMethod();
        case DELETE:
           return runDeleteMethod();
        default:
            return (_request.setStatusCode(NOT_FOUND));
    }
}

int Client::runDeleteMethod()
{
    string uri = _request.getURI();
    string filePath = defineFilePath(uri);

    if (!Utils::fileExists(filePath)) {
        setResponseData(NOT_FOUND, "", "text/plain", "404 Not Found");
        return NOT_FOUND;
    }

    if (!Utils::hasDeletePermission(filePath)) {
        setResponseData(FORBIDDEN, "", "text/plain", "403 Forbidden");
        return FORBIDDEN;
    }

    if (remove(filePath.c_str()) != 0) {
        setResponseData(INTERNAL_SERVER_ERROR, "", "text/plain", "500 Internal Server Error");
        return INTERNAL_SERVER_ERROR;
    }

    setResponseData(OK, filePath, "text/plain", "200 OK");
    return OK;
}

int Client::runGetMethod()
{
    string uri = _request.getURI();
    string filePath = defineFilePath(uri);
    string contentType = defineContentType(filePath);
    string responseBody = defineResponseBody(filePath, uri);
    string contentLength = defineContentLength(responseBody);

    if (_response.getStatusCode() == NOT_FOUND){
        setResponseData(NOT_FOUND, filePath, "text/plain", "404 Not Found");
        return NOT_FOUND;
    } else if (_response.getStatusCode() == FORBIDDEN){
        setResponseData(FORBIDDEN, filePath, "text/plain", "403 Forbidden");
        return FORBIDDEN;
    } else if (_response.getStatusCode() == INTERNAL_SERVER_ERROR){
        setResponseData(INTERNAL_SERVER_ERROR, filePath, "text/plain", "500 Internal Server Error");
        return INTERNAL_SERVER_ERROR;
    }
    setResponseData(OK, filePath, contentType, responseBody);
    return (OK);
}
    // _response.setStatusCode(OK);// TODO: colocar o status code correto conforme o ocorrido
    
static string defineHome(const vector<Route>& routes){
    for (size_t i = 0; i < routes.size(); i++)
        if (routes[i].getRoute() == "/")
            return ("content" + routes[i].getRoot());
    return ("content/index.html");
}

string Client::defineFilePath(string uri)
{   
    /* TODO: reformular essa função, as vezes o cliente solicita algo com '/' no inicio. 
    Especialmente quando usado autoindex, esse é um ponto de atenção */

    string filePath;

    if (uri == "/") {
        filePath = defineHome(_request.getServer().getRoute());
    }
    else if (uri == "/cgi")
    	filePath = "content" + uri + "/" + _request.getServer().getRoute()[0].getIndex();
    else {
        filePath = "content" + uri; // TODO: nem sempre a pasta sera a content, precisa ler e pegar corretamente a pasta conforme a rota
    }
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
    return ("text/plain;charset=UTF-8");
}

string Client::defineResponseBody(const string& filePath, const string& uri)
{

	if (_request.getIsCgi()) {
        if (filePath.find(".py") != string::npos || filePath.find(".php") != string::npos){
            return(_response.executeCGI(_request, filePath));
        }
    }

    struct stat path_stat;
    stat(filePath.c_str(), &path_stat);
    if (S_ISDIR(path_stat.st_mode)){
        if (_response.checkAutoIndexInRoute(_request.getServer().getRoute()))
        	return (_response.handleAutoIndex(filePath, uri));
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

void Client::sendResponse(struct pollfd& pollFds, map<int, Request>& requests)
{
    Client::_request = requests[pollFds.fd];
    string build;

    if (_request.getStatusCode() == BAD_REQUEST) {
        setResponseData(BAD_REQUEST, "", "text/plain", "Bad Request");
        build = _response.buildMessage();
    } else {
        callMethod();
        build = _response.buildMessage();
    }
    send(pollFds.fd, build.c_str(), build.size(), 0);
    cout << "Message sent" << endl;

    requests.erase(pollFds.fd);
    close(pollFds.fd);

    _request.clear();
    _response.clear();
}

bool Client::verifyPermission(const string &file)
{
    if (access(file.c_str(), F_OK) != 0)
        return (false);
    else if (access(file.c_str(), F_OK) != 0)
        return (false);
    return (true);
}

string Client::defineContentLength(const string &body)
{
    ostringstream oss;

    size_t len = body.size();
    oss << len;
    return (oss.str());
}

void Client::setResponseData(int statusCode, string filePath, string contentType, string responseBody) {
    _response.setStatusCode(statusCode);
    _response.setFilePath(filePath);
    _response.setContentType(contentType);
    _response.setResponseBody(responseBody);
    _response.setContentLength(defineContentLength(responseBody));
}