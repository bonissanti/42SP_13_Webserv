#include "../include/Request.hpp"

void	Request::parseRequest(char buffer[], ssize_t bytesReceived)
{
	char delim;
	string token;
	stringstream ss(buffer);
	
	ss >> _method >> delim >> _path >> delim >> _protocol; 
	// while (getline(ss, token, ' '))
	// {
	// 	if (token == "GET")
	// 		cout << "oi" << endl;
	// }	
	(void)bytesReceived;
}

void Request::readRequest(vector<struct pollfd>& pollFds, int i)
{
	Request request;
    char buffer[65535];
    ssize_t bytesReceived = recv(pollFds[i].fd, buffer, sizeof(buffer), 0);
    
    request.parseRequest(buffer, bytesReceived);
    
    if (bytesReceived > 0)
    	cout << "a" << endl;
    else if (bytesReceived == 0)
        cout << "Connection closed" << endl;
    else
        perror("Error: recv failed");

    close(pollFds[i].fd);
    pollFds.erase(pollFds.begin() + i);
}