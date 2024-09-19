#include "Client.hpp"

extern char **environ;

void Client::freeEnviron(char **envp)
{
    for (int i = 0; envp[i]; i++)
        delete[] envp[i];
    delete[] envp;
}

bool Client::checkFile(const string &file)
{
    ifstream f(file.c_str());
    return (f.good());
}

char **Client::configEnviron(Server& server, Request &req)
{
    // Determine the number of environment variables
    int envSize = 7;
    if (!req.getBody().empty()) {
        envSize += 2; // For CONTENT_LENGTH and CONTENT_TYPE
    }

    // Allocate memory for environment variables
    char **envp = new char *[envSize + 1]; // +1 for the NULL terminator

    // Basic environment variables
    string serverName = "SERVER_NAME=" + server.getServerName();
    string serverPort = "SERVER_PORT=" + Utils::itostr(server.getListen());
    string protocol = "SERVER_PROTOCOL=" + _request.getVersion();
    string pathInfo = "PATH_INFO=" + _request.getURI();
    string method = "REQUEST_METHOD=" + req.getMethod();
    string gatewayInterface = "GATEWAY_INTERFACE=CGI/1.1";

    envp[0] = strdup(serverName.c_str());
    envp[1] = strdup(serverPort.c_str());
    envp[2] = strdup(protocol.c_str());
    envp[3] = strdup(pathInfo.c_str());
    envp[4] = strdup(method.c_str());
    envp[5] = strdup(gatewayInterface.c_str());

    // Add CONTENT_LENGTH and CONTENT_TYPE if the body exists
    int index = 6;
    if (!req.getBody().empty()) {
        string contentLength = "CONTENT_LENGTH=" + Utils::itostr(req.getBody().size());
        string contentType = "CONTENT_TYPE=" + req.getHeader("content-type");

        envp[index++] = strdup(contentLength.c_str());
        envp[index++] = strdup(contentType.c_str());
    }

    // Null-terminate the array
    envp[index] = NULL;

    return envp;
}

string Client::executeCGI(Request &req, Server& server, string filePath)
{
    int pid;
    int fd;
    int status;
    string result;
    if (!checkFile(filePath)) {
        return (setPageError(NOT_FOUND, ERROR400));
    }

    size_t found = filePath.find('.');
    if (filePath.substr(found) == ".py")
        _executor = "/usr/bin/python3";
	else
		_executor = "/usr/bin/php";

    pid = fork();
    if (pid < 0) {
        throw Server::exception(RED "Error: Fork failed" RESET);
    }
    if (pid == 0) {

        fd = open("/tmp/tempFile", O_TRUNC | O_CREAT | O_WRONLY, 0644);
        signal(SIGINT, Utils::handleSignals);
        signal(SIGTERM, Utils::handleSignals);
        char **envp = configEnviron(server, req);
        char **args = new char*[4]; // Allocate memory for 4 pointers (3 arguments + 1 NULL terminator)
        if (req.getBody().empty()) {
            args[0] = const_cast<char *>(_executor.c_str());
            args[1] = const_cast<char *>(filePath.c_str());
            args[3] = NULL;
        } else {
            args[0] = const_cast<char *>(_executor.c_str());
            args[1] = const_cast<char *>(filePath.c_str());
            args[2] = const_cast<char *>(req.getBody().c_str());
            args[3] = NULL;
        }
		cout << "Executing: " << _executor << " " << filePath << endl;
		cout << "With args: " << args[0] << " " << args[1] << " " << args[2] << endl;
		dup2(fd, STDOUT_FILENO);
        if (execve(_executor.c_str(), args, envp) == -1) {
            freeEnviron(envp);
            close(fd);
            cerr << RED << "Error: execve failed" << RESET << endl;
            exit(1);
        }
        freeEnviron(envp);
        close(fd);
    }
    else {
        clock_t time = clock();

        while ((float)(clock() - time) / CLOCKS_PER_SEC < 5.0f){
            int waitValue = waitpid(pid, &status, WNOHANG);
            if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                string tempFile = "/tmp/tempFile";
                return(Utils::readFile(tempFile));
            }
            else if (waitValue == -1 && status != -1)  {
            	cerr << RED << "Error: child process failed at CGI execution" << RESET << endl;
                kill(pid, SIGKILL);
                return (setPageError(INTERNAL_SERVER_ERROR, ERROR500));
            }
        }
    }
    kill (pid, SIGKILL);
    return (setPageError(REQUEST_TIMEOUT, ERROR408));
}
