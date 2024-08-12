#include "../Server/Server.hpp"
#include "../Request/Request.hpp"

void	Request::handleCGI(void)
{
	std::cout << "has CGI" << std::endl;
}

	// string requestMethod = "REQUEST_METHOD" += Request.getMethod(); GET / POST / DELETE
	// string contentLength = "CONTENT_LENGTH" += Request.contentSize(); ou .size();
	// string scriptName = "SCRIPT_NAME" += uri completa
	// string serverName = "SERVER_NAME" += Request.getName();
	// string serverPort = "SERVER_PORT" += Request.getPort();
	//string protocol = "SERVER_PROTOCOL=HTTP/1.1";

char **configEnviron(void)
{
	char **envp = new char*[3];
	string requestMethod = "REQUEST_METHOD=GET";
	string queryString = "QUERY_STRING=username=Random&emailaddress=random@example.com";
	envp[0] = strdup(requestMethod.c_str());
	envp[1] = strdup(queryString.c_str());
	envp[2] = NULL;
	return (envp);
}

void	freeEnviron(char **envp)
{
	for (int i = 0; envp[i]; i++)
		delete[] envp[i];
	delete[] envp;
}

void	readCGI(int fd_in)
{
	ssize_t bytesRead;
	char buffer[65535];
	string result;

	while ((bytesRead = read(fd_in, &buffer, sizeof(buffer))) > 0){
		buffer[bytesRead] = '\0';
		result += buffer;
	}
}

inline bool	checkFile(const std::string& file){
	ifstream f(file.c_str());
	return (f.good());
}

// para teste, serão substituidos pela estrutura do request
	// string tempPath = "current_time.py";
	// string tempRootCGI = "/cgi/";
	// string execPath;

void	Request::executeCGI(void)
{
	int pid;
	int fd[2];
	int status;
	string cgiFile;
	string executor;
	// const char *script = "cgi/test_cgi.py";
	
	if (!checkFile(cgiFile))
		throw Server::exception(RED "Error: cgi file not found" RESET);

	size_t found = cgiFile.find('.');
	if (cgiFile.substr(found) == ".py")
		executor = "usr/bin/python3";
	else
		executor = "usr/bin/php"; //:TODO

	pipe(fd);
	pid = fork();
	if (pid < 0){
		throw Server::exception(RED "Error: Fork failed" RESET);
	}
	if (pid == 0){
		char **envp = configEnviron();
		char *args[] = {const_cast<char *>(executor.c_str()), const_cast<char *>(cgiFile.c_str()), NULL};

		dup2(fd[1], STDOUT_FILENO);
		close(fd[0]);
		if (execve(executor.c_str(), args, envp) == -1){
			freeEnviron(envp);
			throw Server::exception(RED "Error: execve failed" RESET);
		}
		close(fd[1]);
	}
	else {
		close(fd[1]);
		readCGI(fd[0]);
		close(fd[0]);
		waitpid(pid, &status, 0);
		if (!WIFEXITED(status) || WEXITSTATUS(status) != 0){
			throw Server::exception(RED "Error: child process failed" RESET);
		}
		
	}
}
