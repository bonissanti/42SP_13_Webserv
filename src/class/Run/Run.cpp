#include "Run.hpp"

#include "../Client/Client.hpp"
#include "../Request/Request.hpp"
#include "../Response/Response.hpp"

Run::Run() {}
Run::~Run() {}

void setServersNumber(string filePath)
{
     if(Utils::validateFile(filePath) == false)
        throw Server::exception("Error: invalid file format");

    ifstream file(filePath.c_str());
    if (!file.is_open()) {
        return -1;
    }

    string line;
    int serverCount = 0;
    bool insideServerBlock = false;
    stack<char> brackets;

    while (getline(file, line)) {
        for (string::size_type i = 0; i < line.size(); ++i) {
            char c = line[i];

            if (!insideServerBlock && line.substr(i, 6) == "server") {
                insideServerBlock = true;
                i += 5;
                continue;
            }

            if (insideServerBlock) {
                if (c == '{') {
                    brackets.push(c);
                }
                else if (c == '}') { if (brackets.empty()) { serverCount = -1;
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

int Run::acceptNewConnection(int serverSocket, vector<struct pollfd>& pollFds)
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

    int flags = fcntl(clientFd, F_GETFL);
    if (flags < 0)
        throw Server::exception(RED "Error: fcntl failed" RESET);
    if (fcntl(clientFd, F_SETFL, flags | O_NONBLOCK) < 0)
        throw Server::exception(RED "Error: fcntl failed" RESET);

    struct pollfd commFd;

    commFd.fd = clientFd;
    commFd.events = POLLIN | POLLOUT;
    commFd.revents = 0;
    pollFds.push_back(commFd);
    return (clientFd);
}

void Run::startServer(vector<Server>& servers)
{
    int returnValue;
    vector<struct pollfd> pollFds = setPollFd(servers);
    vector<Client> clientManager;
    // Client clientManager;
    // map<int, Request> requests

    while (true) {
        returnValue = poll(pollFds.data(), pollFds.size(), 60 * 1000);

        if (returnValue == 0) {
            cout << "Error: poll Timeout" << endl;
            // Jogar pagine de timeout
        }
        else if (returnValue == -1) {
            cout << "Error: poll failed" << endl;
        }
        else {
            for (size_t i = 0; i < pollFds.size(); i++) {
                if ((pollFds[i].revents & POLLIN) && (i < servers.size())) {
                    int clientFd = acceptNewConnection(pollFds[i].fd, pollFds);
                    // clientManager.addAssociation(clientFd, servers[i]);
                }
                else if (pollFds[i].revents & POLLIN) {
                    Server actualServer = clientManager.getServerFd(pollFds[i].fd);
                    try {
                        Request::readRequest(pollFds, i, requests);
                    }
                    catch (const std::exception& e) {
                        cerr << "Error reading request: " << e.what() << endl;
                    }
                }
                else if (pollFds[i].revents & POLLOUT) {
                    if (requests.find(pollFds[i].fd) != requests.end() && requests[pollFds[i].fd].isReadyForResponse())
                        try {
                            clientManager.sendResponse(pollFds[i], requests);
                        }
                        catch (const std::exception& e) {
                            cerr << "Error sending response: " << e.what() << endl;
                        }
                }
            }
        }
    }
}
