#include "Run.hpp"
#include "../Request/Request.hpp"
#include "../Client/Client.hpp"

vector<struct pollfd> Run::loadPolls(vector<Server> servers)
{
    vector<struct pollfd> pollFds(servers.size());
    Request request;
    Client client;

    for (size_t i = 0; i < servers.size(); i++) {
        pollFds[i].fd = servers[i]._socketFd;
        pollFds[i].events = POLLIN | POLLOUT;
    }
    return (pollFds);
}

void Run::startServer(vector<Server>& servers)
{
    Client client; // temp
    Request request; // temp
    int returnValue;

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
                    request.readRequest(pollFds, i);
                }
                else if (pollFds[i].revents & POLLOUT) {
                    client.sendResponse(request);
                    pollFds.erase(pollFds.begin() + i);
                }
            }
            break;
        }
    }
}

int acceptNewConnection(int serverSocket, vector<struct pollfd>& pollFds)
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