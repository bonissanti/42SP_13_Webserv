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
    int returnValue;

    while (true) {
        returnValue = poll(pollFds.data(), pollFds.size(), 60 * 1000);
        switch (returnValue) {
            case 0:
                cout << "Error: poll Timeout" << endl;
                // jogar pagina de timeout
                break;

            case -1:
                cout << "Error: poll failed" << endl;
                break;

            default:
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
            perror("Error: accept failed");
    }
    else
        cout << "New communication established!" << endl;  // log message

    struct pollfd commFd;

    commFd.fd = clientFd;
    commFd.events = POLLIN | POLLOUT;
    commFd.revents = 0;
    pollFds.push_back(commFd);
}