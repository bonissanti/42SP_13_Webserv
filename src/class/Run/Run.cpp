#include "Run.hpp"
#include <sys/poll.h>
#include "../Request/Request.hpp"
#include "../Client/Client.hpp"

Run::Run(){}
Run::~Run(){}

vector<struct pollfd> Run::loadPolls(vector<Server> servers)
{
    vector<struct pollfd> pollFds(servers.size());
    Client client;

    for (size_t i = 0; i < servers.size(); i++) {
        pollFds[i].fd = servers[i].getSocket();
        pollFds[i].events = POLLIN | POLLOUT;
    }
    return (pollFds);
}


static void sendResponse(vector<struct pollfd>& pollFds, int i, map<int, Request>& requests)
{
	Request &req = requests[pollFds[i].fd];
	string hello =
    	"HTTP/1.1 200 OK\r\n"
    	"Content-Type: text/plain\r\n"
    	"Content-Length: 17\r\n"
    	"Connection: close\r\n"
    	"\r\n"
    	"Hello from server";

    send(pollFds[i].fd, hello.c_str(), hello.size(), 0);
    cout << "Message sent" << endl;
    
    requests.erase(pollFds[i].fd);
    close(pollFds[i].fd);
    pollFds.erase(pollFds.begin() + i);
    (void)req;
}

void readRequest(vector<struct pollfd>& pollFds, int i, map<int, Request>& requests)
{
    char buffer[65535];
    ssize_t bytesReceived = recv(pollFds[i].fd, buffer, sizeof(buffer), 0);
    if (bytesReceived > 0) {
      	requests[pollFds[i].fd] = Request(buffer);
    } 
    else if (bytesReceived == 0) {
        cout << "Connection closed" << endl;
        close(pollFds[i].fd);
        pollFds.erase(pollFds.begin() + i);
        requests.erase(pollFds[i].fd);
    } 
    else {
    	cerr << "Error: recv failed" << endl;
        requests.erase(pollFds[i].fd);
    }
    close(pollFds[i].fd);
    pollFds.erase(pollFds.begin() + i);
}

void acceptNewConnection(int serverSocket, vector<struct pollfd>& pollFds)
{
    int clientFd;
    socklen_t addrlen;
    struct sockaddr_in clientAddr;

    addrlen = sizeof(clientAddr);
    clientFd = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrlen);
    if (clientFd == -1) {
        if (errno == EWOULDBLOCK)
            cout << "No pending connections for now" << endl;
        else
        	cerr << "Error: accept failed" << endl;
    }
    else
        cout << "New communication established!" << endl;  // log message

    struct pollfd commFd;

    commFd.fd = clientFd;
    commFd.events = POLLIN | POLLOUT;
    commFd.revents = 0;
    pollFds.push_back(commFd);
}

void Run::startServer(vector<Server>& servers)
{
    int returnValue;
    map<int, Request> requests;
    vector <struct pollfd> pollFds = loadPolls(servers);

    while (true) {
        returnValue = poll(pollFds.data(), pollFds.size(), 60 * 1000);

        if (returnValue == 0){
            cout << "Error: poll Timeout" << endl;
            //Jogar pagine de timeout
        }
        else if (returnValue == -1){
            cout << "Error: poll failed" << endl;
            break;
        }
        else{
            for (size_t i = 0; i < pollFds.size(); i++) {
                if ((pollFds[i].revents & POLLIN) && (i < servers.size())) {
                	acceptNewConnection(pollFds[i].fd, pollFds);
                }
                else if (pollFds[i].revents & POLLIN) {
                    readRequest(pollFds, i, requests);
                }
                else if (pollFds[i].revents & POLLOUT) {
                	if (requests.find(pollFds[i].fd) != requests.end()) 
                    	sendResponse(pollFds, i, requests);
                }
            }
        }
    }
}
