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

char **Client::configEnviron(Server& server, Request &req, string &filePath)
{
    int envSize = 8;
    if (!req.getBody().empty()) {
        envSize += 2;
    }

    char **envp = new char *[envSize + 1];

    // Basic environment variables
    string serverName = "SERVER_NAME=" + server.getServerName();
    string serverPort = "SERVER_PORT=" + Utils::itostr(server.getListen());
    string protocol = "SERVER_PROTOCOL=" + _request.getVersion();
    string pathInfo = "PATH_INFO=" + filePath;
    string method = "REQUEST_METHOD=" + req.getMethod();
    string gatewayInterface = "GATEWAY_INTERFACE=CGI/1.1";
	string scriptName = "SCRIPT_NAME=" + filePath.substr(filePath.find_last_of('/') + 1, filePath.length() - filePath.find_last_of('/'));

    envp[0] = strdup(serverName.c_str());
    envp[1] = strdup(serverPort.c_str());
    envp[2] = strdup(protocol.c_str());
    envp[3] = strdup(pathInfo.c_str());
    envp[4] = strdup(method.c_str());
    envp[5] = strdup(gatewayInterface.c_str());
	envp[6] = strdup(scriptName.c_str());

    // Add CONTENT_LENGTH and CONTENT_TYPE if the body exists
    int index = 7;
    if (!req.getBody().empty()) {
        string contentLength = "CONTENT_LENGTH=" + Utils::itostr(req.getBody().size());
        string contentType = "CONTENT_TYPE=" + req.getHeader("content-type");
		string queryString = "QUERY_STRING=" + req.getBody();

        envp[index++] = strdup(contentLength.c_str());
        envp[index++] = strdup(contentType.c_str());
		envp[index++] = strdup(queryString.c_str());
    }

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
        return (setPageError(NOT_FOUND));
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
        // Processo filho - executa o CGI
        fd = open("/tmp/tempFile", O_TRUNC | O_CREAT | O_WRONLY, 0644);
        signal(SIGINT, Utils::handleSignals);
        signal(SIGTERM, Utils::handleSignals);

        // Configura as variáveis de ambiente (envp) corretamente
        char **envp = configEnviron(server, req, filePath);

        // Prepara os argumentos do execve
        char **args = new char*[3];
        args[0] = const_cast<char *>(_executor.c_str());
        args[1] = const_cast<char *>(filePath.c_str());
        args[2] = NULL;

        // Redireciona a saída para o arquivo temporário
        dup2(fd, STDOUT_FILENO);
        
        // Se for uma requisição POST, redireciona o corpo da requisição para o stdin
        // if (req.getMethod() == "POST") {
        //     string body = req.getBody();
        //     int stdin_fd = open("/tmp/tempStdin", O_CREAT | O_WRONLY | O_TRUNC, 0644);
        //     write(stdin_fd, body.c_str(), body.size());
        //     close(stdin_fd);
        //     stdin_fd = open("/tmp/tempStdin", O_RDONLY);
        //     dup2(stdin_fd, STDIN_FILENO);  // Redireciona o STDIN
        //     close(stdin_fd);
        // }

        // Executa o CGI (PHP ou Python)
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
                return (setPageError(INTERNAL_SERVER_ERROR));
            }
        }
    }
    kill (pid, SIGKILL);
    return (setPageError(REQUEST_TIMEOUT));
}
