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
    if (_request.getPath().empty())
        return (_request.setStatusCode(BAD_REQUEST));

	switch (getMethodIndex(_request.getMethod()))
    {
        case GET:
           return runGetMethod();
        case POST:
           return runPostMethod();
        case DELETE:
        //    return runDeleteMethod(req);
        default:
            return (_request.setStatusCode(NOT_FOUND));
    }
}

int Client::runGetMethod()
{
    string filePath = _request.getPath();
    _response.setFilePath(filePath);

    if (filePath.length() - 1 == '/')
        filePath += "index.html";

    string contentType = _response.defineContentType(filePath); 
    string responseBody = defineResponseBody(this->_request);
    size_t contentLength = _response.defineContentLength(responseBody); 

    _response.setContentType(contentType);
    _response.setResponseBody(responseBody);
    _response.setContentLength(contentLength);
}

void Client::sendResponse(struct pollfd& pollFds, map<int, Request>& requests)
{
    _request = requests[pollFds.fd];

    int statusCode = callMethod();
    // if (statusCode == NOT_FOUND)
    // {

    // }

    string hello =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 17\r\n"
        "Connection: close\r\n"
        "\r\n"
        "Hello from server";

    send(pollFds.fd, hello.c_str(), hello.size(), 0);
    cout << "Message sent" << endl;

    requests.erase(pollFds.fd);
    close(pollFds.fd);
}
