#include "Response.hpp"
#include "../../../include/Utils.hpp"
#include <sys/stat.h>

Response::Response() {}

Response::~Response() {}

static bool verifyPermission(const string &file)
{
    if (access(file.c_str(), F_OK) != 0)
        return (false);
    else if (access(file.c_str(), F_OK) != 0)
        return (false);
    return (true);
}

// string Response::defineFilePath(string& uri, Request& req){
//     string filePath;
//     // (void)req;

//     if (uri.find("..") != string::npos) {
//         _statusCode = FORBIDDEN;
//         return ("");
//     }
//     filePath = req.getServer().getRoot() + uri;
//     cout << filePath << endl;
//     if (uri == "/")
//         filePath += "content/html/index.html";
//     return (filePath);
// }

string Response::defineFilePath(string &uri, Request &req)
{
    string filePath;
    (void)req;

    if (uri == "/") {
        cout << "here" << endl;
        filePath = "content/index.html";
    }
    else {
        filePath = "content" + uri; // TODO: nem sempre a pasta sera a content, precisa ler e pegar corretamente a pasta conforme a rota
    }
    return (filePath);
}

string Response::defineResponseBody(const string &filePath, Request &req)
{
    if (req._isCgi) {
        _index = req.getServer().getRoute()[0].getIndex();

        if (_index.find(".py") != string::npos || _index.find(".php") != string::npos)
            return (this->executeCGI(req));
    }

    ifstream file(filePath.c_str());
    if (!file.is_open()) {
        _statusCode = NOT_FOUND;
        return ("");
    }
    else if (file.fail()) {
        _statusCode = INTERNAL_SERVER_ERROR;
        return ("");
    }
    stringstream buffer;
    buffer << file.rdbuf();

    if (!verifyPermission(filePath)) {
        _statusCode = FORBIDDEN;
        return ("");
    }
    file.close();
    return (buffer.str());
}

string Response::defineContentLength(const string &body)
{
    ostringstream oss;

    size_t len = body.size();
    oss << len;
    return (oss.str());
}

string Response::defineContentType(string filePath)
{
    size_t index;
    string extension;
    map<string, string> mimeTypes;
    mimeTypes[".html"] = "text/html";
    mimeTypes[".css"] = "text/css";
    mimeTypes[".txt"] = "text/plain";
    mimeTypes[".png"] = "image/png";
    mimeTypes[".jpg"] = "image/jpg";
    mimeTypes[".gif"] = "image/gif";
    mimeTypes[".js"] = "application/js";
    mimeTypes[".pdf"] = "application/pdf";

    index = filePath.rfind('.');
    if (index != string::npos) {
        extension = filePath.substr(index);
        map<string, string>::iterator it = mimeTypes.begin();

        for (; it != mimeTypes.end(); ++it)
            if (it->first == extension)
                return (it->second + ";charset=UTF-8");
    }
    return ("text/plain;charset=UTF-8");
}

string Response::buildMessage(void)
{
    // string response =
    //     "HTTP/1.1 200 OK\r\n"
    //     "Content-Type: text/plain\r\n"
    //     "Content-Length: 17\r\n"
    //     "Connection: close\r\n"
    //     "\r\n"
    //     "Hello from server";W

    string response =
        "HTTP/1.1 " + Utils::statusCodeToString(_statusCode) + "\r\n"
        "Access-Control-Allow-Credentials: true\r\n"
        "Access-Control-Allow-Headers: Content-Type, Authorization\r\n"
        "Access-Control-Allow-Methods: DELETE, GET, POST\r\n"
        "Content-Length:" + _contentLength +
        "\r\n"
        "Content-Type:" + _contentType +
        "\r\n"
        "Connection: close" +
        "\r\n"
        "\r\n" +
        _responseBody;

    return (response);
}

void Response::freeEnviron(char **envp)
{
    for (int i = 0; envp[i]; i++)
        delete[] envp[i];
    delete[] envp;
}

bool Response::checkFile(const string &file)
{
    ifstream f(file.c_str());
    return (f.good());
}

char **Response::configEnviron(Request &req)
{
    char **envp = new char *[3];
    string requestMethod = "REQUEST_METHOD=GET" + req._method;
    // string queryString = "QUERY_STRING=" + ex: username=Random&emailaddress=random@example.com;
    string contentType = "CONTENT_TYPE=" + _contentType;
    // string contentLength = "CONTENT_LENGTH=" + _contentLength;
    // string scriptName = "SCRIPT_NAME=" + /cgi/filename presente dentro do req.server.route
    // string pathInfo = "PATH_INFO=" +
    string serverName = "SERVER_NAME=" + req.getServer().getServerName();
    // string serverPort = "SERVER_PORT=" + req.getServer().getListen()
    string protocol = "SERVER_PROTOCOL=" + req._version;

    envp[0] = strdup(requestMethod.c_str());
    // envp[1] = strdup(queryString.c_str());
    envp[2] = NULL;
    return (envp);
}

string Response::executeCGI(Request &req)
{
    int pid;
    int fd[2];
    int status;
    string result;

    if (!checkFile(_index)) {
        _statusCode = NOT_FOUND;
        return ("");
    }

    size_t found = _index.find('.');
    if (_index.substr(found) == ".py")
        _executor = "usr/bin/python3";
    else
        _executor = "usr/bin/php";

    pipe(fd);
    pid = fork();
    if (pid < 0) {
        throw Server::exception(RED "Error: Fork failed" RESET);
    }
    if (pid == 0) {
        char **envp = configEnviron(req);
        char *args[] = {const_cast<char *>(_executor.c_str()), const_cast<char *>(_index.c_str()), NULL};

        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        if (execve(_executor.c_str(), args, envp) == -1) {
            freeEnviron(envp);
            throw Server::exception(RED "Error: execve failed" RESET);
        }
        close(fd[1]);
    }
    else {
        close(fd[1]);
        result = readCGI(fd[0]);
        close(fd[0]);
        waitpid(pid, &status, 0);
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            throw Server::exception(RED "Error: child process failed" RESET);
        }
    }
    return (result);
}

/*  TODO: QUERY_STRING = Parsear o que vem do path http://localhost:8080/search?query=livros&categoria=ficcao&ordem=asc)
separando pelo '?'.
    CONTENT_LENGTH = converter de size_t para string
    PATH_INFO = separar o que seria 'search?' do uri acima, o delimitador creio que seja sempre o '?'
    SCRIPT_NAME = concatenar o /cgi/ com o filename.py ou php, ambos estão no route dentro do server
    SERVER_NAME = criar o getServerName()
    SERVER_PORT = criar o getListen()
*/

string Response::readCGI(int fd_in)
{
    ssize_t bytesRead;
    char buffer[65535];
    string result;

    while ((bytesRead = read(fd_in, &buffer, sizeof(buffer))) > 0) {
        buffer[bytesRead] = '\0';
        result += buffer;
    }
    return (result);
}

void Response::clear() {
    _statusCode = 0;
    _index.clear();
    _executor.clear();
    _statusMessage.clear();
    _responseBody.clear();
    _filePath.clear();
    _contentType.clear();
    _contentLength.clear();
    _headers.clear();
}
