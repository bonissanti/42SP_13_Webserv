#include "Run.hpp"

#include "../Client/Client.hpp"
#include "../Request/Request.hpp"
#include "../Response/Response.hpp"

Run::Run() {}
Run::~Run() {}

Client* Run::getClientFd(int clientFd)
{
    Client* clientPtr = NULL;
    if (_mapClient.find(clientFd) != _mapClient.end())
        return (_mapClient[clientFd]);
    return (clientPtr);
}

int Run::setServersNumber(string filePath)
{
    if (Utils::validateFile(filePath) == false)
        throw Server::exception(RED "Error: invalid file format" RESET);

    ifstream file(filePath.c_str());
    if (!file.is_open()) {
        return -1;
    }

    string rawLine, line;
    int serverCount = 0;
    bool insideServerBlock = false;
    // bool listenFound = false;
    // bool allowedMethodsFound = false;
    stack<char> brackets;

    while (getline(file, rawLine)) {
        line = Utils::trim(rawLine);
        for (string::size_type i = 0; i < line.size(); ++i) {
            char c = line[i];
            if (!insideServerBlock && line.substr(i, 6) == "server") {
                insideServerBlock = true;
                i += 5;
                continue;
            }
            // else if (insideServerBlock && line.substr(i, 6) == "listen"){
            //     listenFound = true;
            // }
            else if (insideServerBlock)
                if (insideServerBlock) {
                    if (c == '{') {
                        brackets.push(c);
                    }
                    else if (c == '}') {
                        if (brackets.empty()) {
                            serverCount = -1;
                        }
                        brackets.pop();
                        if (brackets.empty()) {
                            insideServerBlock = false;
                            ++serverCount;
                        }
                    }
                }
        }
    }
    if (serverCount == -1 || serverCount > 1024)
        throw Server::exception(RED "Error: invalid config file" RESET);
    return serverCount;
}

int acceptNewConnection(int serverSocket, vector<struct pollfd>& pollFds)
{
    int clientFd;
    socklen_t addrlen;
    struct sockaddr_in clientAddr;

    addrlen = sizeof(clientAddr);
    clientFd = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrlen);
    if (clientFd == -1) {
        cerr << RED << "Error: accept failed" << RESET << endl;
    }
    else
        cout << GREEN << "New communication established!" << RESET << endl;  // log message

    struct pollfd commFd;

    commFd.fd = clientFd;
    commFd.events = POLLIN | POLLOUT;
    commFd.revents = 0;
    pollFds.push_back(commFd);
    return (clientFd);
}

vector<struct pollfd> loadPolls(vector<Server> servers)
{
    vector<struct pollfd> pollFds(servers.size());

    for (size_t i = 0; i < servers.size(); i++) {
        pollFds[i].fd = servers[i].getSocket();
        pollFds[i].events = POLLIN | POLLOUT;
    }
    return (pollFds);
}

void Run::startServer(vector<Server>& servers)
{
    int returnValue;
    map<int, Request> mapRequests;
    vector<struct pollfd> pollFds = loadPolls(servers);
    Client client;

    while (true) {
        returnValue = poll(pollFds.data(), pollFds.size(), 60 * 1000);

        if (signalUsed) {
            break;
        }
        else if (returnValue == -1) {
            break;
        }
        else {
            for (size_t i = 0; i < pollFds.size(); i++) {
                if ((pollFds[i].revents & POLLIN) && (i < servers.size())) {
                    int clientFd = acceptNewConnection(pollFds[i].fd, pollFds);
                    client.addAssociation(clientFd, servers[i]);
                }
                else if (pollFds[i].revents & POLLIN) {
                    Server* actualServer = client.getServerFd(pollFds[i].fd);
                    Request::readRequest(pollFds, i, mapRequests, *actualServer);
                }
                else {
                    if (mapRequests.find(pollFds[i].fd) != mapRequests.end()) {
                        Request& request = mapRequests[pollFds[i].fd];

                        if (request.getIsReadyForResponse()) {
                            client.sendResponse(pollFds[i], mapRequests);
                            pollFds.erase(pollFds.begin() + i);
                        }
                    }
                }
            }
        }
    }
}