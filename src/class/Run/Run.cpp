#include "Run.hpp"
#include <sys/poll.h>
#include "../Request/Request.hpp"
#include "../Response/Response.hpp"

Run::Run(){}
Run::~Run(){}

vector<struct pollfd> Run::loadPolls(vector<Server> servers)
{
    vector<struct pollfd> pollFds(servers.size());

    for (size_t i = 0; i < servers.size(); i++) {
        pollFds[i].fd = servers[i].getSocket();
        pollFds[i].events = POLLIN | POLLOUT;
    }
    return (pollFds);
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
    
    Server svConnection = servers[0];
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
                    Request::readRequest(pollFds, i, requests);
                }
                else if (pollFds[i].revents & POLLOUT) {
                	if (requests.find(pollFds[i].fd) != requests.end()) 
                    	Response::sendResponse(pollFds, i, requests);
                }
            }
        }
    }
}
