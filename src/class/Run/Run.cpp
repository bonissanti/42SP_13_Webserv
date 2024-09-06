#include "Run.hpp"

#include "../Client/Client.hpp"
#include "../Request/Request.hpp"
#include "../Response/Response.hpp"

Run::Run() {}
Run::~Run() {}

int Run::setServersNumber(string filePath)
{
    if (Utils::validateFile(filePath) == false)
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

pollfd Run::acceptNewConnection(int socketFd)
{
    int clientFd;
    socklen_t addrlen;
    struct sockaddr_in clientAddr;

    addrlen = sizeof(clientAddr);
    clientFd = accept(socketFd, (struct sockaddr*)&clientAddr, &addrlen);
    if (clientFd == -1) {
        if (errno == EWOULDBLOCK)
            cout << "No pending connections for now" << endl;
        else
            cerr << RED << "Error: accept failed" << RESET << endl;
    }
    else
        cout << "New communication established!" << endl;  // log message

    // int flags = fcntl(clientFd, F_GETFL, 0);
    // if (flags == -1)
    //     cerr << RED << "Error: fcntl failed" << RESET << endl;
    // if (fcntl(clientFd, F_SETFL, flags | O_NONBLOCK) == -1)
    //     cerr << RED << "Error: fcntl failed" << RESET << endl;

    struct pollfd commFd;

    commFd.fd = clientFd;
    commFd.events = POLLIN | POLLOUT;
    commFd.revents = 0;
    return (commFd);
}

void Run::startServer(vector<Server>& servers)
{
    int pollValue;
    while (true) {
        Client client;
        bool requestFound = false;
        size_t i = 0;

        if (signalUsed){
            break ;
        }
        while (i < servers.size()) {
            pollValue = poll(&servers[i].getPollFd(), 1, 10);
            if (signalUsed){
                break ;
            }
            if (pollValue == -1)
                throw Server::exception(RED "Error: poll failed" RESET);
            if (servers[i].getPollFd().revents & POLLIN) {
                try {
                    requestFound = true;
                    struct pollfd actualFd = Run::acceptNewConnection(servers[i].getPollFd().fd);
                    servers[i].setClientFd(actualFd);
                    client.setServer(servers[i]);
                    client.getRequest()->readRequest(actualFd);
                }
                catch (const std::exception& e) {
                    cerr << "Error reading request: " << e.what() << endl;
                }
            }
            else if (servers[i].getPollFd().revents & POLLOUT) {
                try {
                    requestFound = false;
                    client.sendResponse();
                    servers[i].getServerFd();
                }
                catch (const std::exception& e) {
                    cerr << "Error sending response: " << e.what() << endl;
                }
            }
            i = requestFound ? i : i + 1;
        }
    }
}