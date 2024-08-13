#include <string>
#include "../Server/Server.hpp"
#include "../Response/Response.hpp"

string	getQueryString(string path){
	size_t pos = path.find('?');
	if (pos != string::npos){
		// string newQuery = 
	}
	return ("");
}
	
/*  TODO: QUERY_STRING = Parsear o que vem do path http://localhost:8080/search?query=livros&categoria=ficcao&ordem=asc)
separando pelo '?'. 
	CONTENT_LENGTH = converter de size_t para string
	PATH_INFO = separar o que seria 'search?' do uri acima, o delimitador creio que seja sempre o '?'
	SCRIPT_NAME = concatenar o /cgi/ com o filename.py ou php, ambos estão no route dentro do server
	SERVER_NAME = criar o getServerName()
	SERVER_PORT = criar o getListen()
*/
char **Response::configEnviron(Request& req)
{
	char **envp = new char*[3];
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

void	freeEnviron(char **envp)
{
	for (int i = 0; envp[i]; i++)
		delete[] envp[i];
	delete[] envp;
}

string	readCGI(int fd_in)
{
	ssize_t bytesRead;
	char buffer[65535];
	string result;

	while ((bytesRead = read(fd_in, &buffer, sizeof(buffer))) > 0){
		buffer[bytesRead] = '\0';
		result += buffer;
	}
	return (result);
}

inline bool	checkFile(const std::string& file){
	ifstream f(file.c_str());
	return (f.good());
}

// para teste, serão substituidos pela estrutura do request
	// string tempPath = "current_time.py";
	// string tempRootCGI = "/cgi/";
	// string execPath;

string	Response::executeCGI(Request& req)
{
	int pid;
	int fd[2];
	int status;
	string result;	
	
	if (!checkFile(_index)){
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
	if (pid < 0){
		throw Server::exception(RED "Error: Fork failed" RESET);
	}
	if (pid == 0){
		char **envp = configEnviron(req);
		char *args[] = {const_cast<char *>(_executor.c_str()), const_cast<char *>(_index.c_str()), NULL};

		dup2(fd[1], STDOUT_FILENO);
		close(fd[0]);
		if (execve(_executor.c_str(), args, envp) == -1){
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
		if (!WIFEXITED(status) || WEXITSTATUS(status) != 0){
			throw Server::exception(RED "Error: child process failed" RESET);
		}
	}
	return (result);
}
