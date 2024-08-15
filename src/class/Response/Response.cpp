#include "Response.hpp"

Response::Response() {}

Response::~Response() {}

static bool verifyPermission(const string& file)
{
    if (access(file.c_str(), F_OK) != 0)
        return (false);
    else if (access(file.c_str(), F_OK) != 0)
        return (false);
    return (true);
}

string Response::defineResponseBody(Request& req)
{
    if (req._isCgi) {
        _index = req.getServer().getRoute()[0].getIndex();

        if (_index.find(".py") != string::npos || _index.find(".php") != string::npos)
            return (executeCGI(req));
    }

    ifstream file(_filePath.c_str());
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

    if (verifyPermission(buffer.str())) {
        _statusCode = FORBIDDEN;
        return ("");
    }
    file.close();
    return (buffer.str());
}

size_t Response::defineContentLength(const string& body)
{
    ostringstream oss;

    oss << body;
    return (oss.str().size());
}


char** Response::configEnviron(Request& req)
{
    char** envp = new char*[3];
    string requestMethod = "REQUEST_METHOD=GET" + req._method;
    // string queryString = "QUERY_STRING=" + ex: username=Random&emailaddress=random@example.com;
    string contentType = "CONTENT_TYPE=" + _contentType;
    // string contentLength = "CONTENT_LENGTH=" + _contentLength;
    // string scriptName = "SCRIPT_NAME=" + /cgi/filename presente dentro do req.server.route
    // string pathInfo = "PATH_INFO=" +
    // string serverName = "SERVER_NAME=" + req.getServer().getServerName()
    // string serverPort = "SERVER_PORT=" + req.getServer().getListen()
    string protocol = "SERVER_PROTOCOL=" + req._version;

    envp[0] = strdup(requestMethod.c_str());
    // envp[1] = strdup(queryString.c_str());
    envp[2] = NULL;
    return (envp);
}

string Response::executeCGI(Request& req)
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
        char** envp = configEnviron(req);
        char* args[] = {const_cast<char*>(_executor.c_str()), const_cast<char*>(_index.c_str()), NULL};

        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        if (execve(_executor.c_str(), args, envp) == -1) {
            freeEnviron(envp);
            throw Server::exception(RED "Error: execve failed" RESET);
        }
        freeEnviron(envp);
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
            	return (it->second + "; charset=UTF-8");
    }
    // string ret = + "; charset=UTF-8"; 
    return ("text/plain");
}


void freeEnviron(char** envp)
{
    for (int i = 0; envp[i]; i++)
        delete[] envp[i];
    delete[] envp;
}

string readCGI(int fd_in)
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

inline bool checkFile(const std::string& file)
{
    ifstream f(file.c_str());
    return (f.good());
}

string getQueryString(string path)
{
    size_t pos = path.find('?');
    if (pos != string::npos) {
        // string newQuery =
    }
    return ("");
}
// void Response::setStatusCode(int code)
// {
//     _statusCode = code;
//     _statusMessage = getStatusMessage(code);
// }

// string Response::getStatusMessage(int code) const
// {
//     switch (code) {
//         // talvez tenha em excesso
//         case 200:
//             return ("OK");
//         case 400:
//             return ("Bad request");
//         case 403:
//             return ("Forbidden");
//         case 404:
//             return ("Not found");
//         case 405:
//             return ("Method not allowed");
//         case 500:
//             return ("Internal server error");
//         case 502:
//             return ("Bad gateway");
//         default:
//             return ("Unknown status");
//     }
// }