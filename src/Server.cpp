#include "../include/Server.hpp"

#include "../include/Route.hpp"
#include "../include/Request.hpp"
#include <map>

Server::Server()
{
    _listen = 100;
    _client_max_body_size = 2 * MB;
    _server_name = "default";
    _root = "/data/";
}

Server::~Server() {}

Server::Server::exception::exception(const string& msg) : msg(msg) {}
Server::Server::exception::~exception() throw() {}

const char* Server::Server::exception::what() const throw()
{
    return (this->msg.c_str());
}

void Server::create(ifstream& file)
{
    bool routeFound = false;

    string line;
    while (getline(file, line)) {
        // test if last line is a last route
        if (routeFound and line.substr(0, 1) == "}" and line.length() == 1)
            break;

        line = Utils::trim(line);
        if (line.empty() or line[0] == '#') {
            continue;
        }

        if ((line.substr(0, 2) == "}," and line.length() == 2) or (line.substr(0, 1) == "}" and line.length() == 1))
            break;

        string key, value;
        istringstream ss(line);
        if (getline(ss, key, '=') && getline(ss, value)) {
            key = Utils::trim(key);
            value = Utils::trim(value);

            if (key == "listen")
                setListen(Utils::strtoi(value.c_str()));
            else if (key == "server_name")
                setServerName(value);
            else if (key == "root")
                _root = setRoot(value);
            else if (key == "client_max_body_size")
                setClientMaxBodySize(value);
            else if (key == "error_page")
                setErrorPage(value);
            else
                throw Server::exception("Unknown configuration key: " + key);
        }
        else if (line.find("route") == 0) {
            Route new_route;
            new_route.create(line, file);
            _routes.push_back(new_route);
            routeFound = true;
        }
    }

    if (_listen == 100)
        throw Server::exception(RED "Error: listen is not set" RESET);
}

void Server::setListen(int port)
{
    try {
        if (port < 1024 || port > 49151)
            throw Server::exception(RED "Error: port number outside of the range. e.g.: 1024 ~ 49151");
    }
    catch (const exception& e) {
        cerr << e.what() << '\n';
    }
    _listen = port;
}

void Server::setServerName(string name)
{
    _server_name = name;
}

string setRoot(string root)
{
    if (root[0] != '/')
        throw Server::exception(RED "Error: misformatted root path, please use '/path'" RESET);
    if (root.substr(root.length() - 1) != "/")
        root.insert(root.end(), '/');
    return (root);
}

void Server::setClientMaxBodySize(string size)
{
    string unit;
    size_t pos = size.find_first_of("kbKBmbMBgbGB");

    unit = size.substr(pos);
    try {
        switch (unit[0]) {
            case 'k':
            case 'K':
                _client_max_body_size = Utils::strtoi(size.substr(0, pos));
                break;

            case 'm':
            case 'M':
                _client_max_body_size = Utils::strtoi(size.substr(0, pos)) * MB;
                break;

            case 'g':
            case 'G':
                _client_max_body_size = Utils::strtoi(size.substr(0, pos)) * GB;
                break;

            default:
                throw Server::exception(RED "Error: unit not recognized" RESET);
        }
    }
    catch (const exception& e) {
        cout << e.what() << endl;
    }
}

void Server::setErrorPage(string error_page)
{
    size_t pos = error_page.find_first_of(" ");
    if (pos == string::npos) {
        throw invalid_argument("Error: invalid error_page format");
    }
    int key = Utils::strtoi(Utils::trim(error_page.substr(0, pos)));
    string value = Utils::trim(error_page.substr(pos + 1));

    map<int, string> mapErrorPage;
    mapErrorPage[key] = value;
    _error_page.push_back(mapErrorPage);
}

void Server::startServer(vector<Server>& servers)
{
    int inUse = 1;
    struct sockaddr_in serverAddr;

    bzero(&serverAddr, sizeof(serverAddr)); // Não pode usar, função C.
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    for (size_t i = 0; i < servers.size(); i++) {
        serverAddr.sin_port = htons(servers[i]._listen);
        servers[i]._socketFd = socket(AF_INET, SOCK_STREAM, 0);

        if (setsockopt(servers[i]._socketFd, SOL_SOCKET, SO_REUSEADDR, &inUse, sizeof(int)) == -1)
            throw Server::exception(RED "Error: setsockopt failed" RESET);

        int flags = fcntl(servers[i]._socketFd, F_GETFL);
        if (flags < 0)
            throw Server::exception(RED "Error: fcntl failed" RESET);

        if (fcntl(servers[i]._socketFd, F_SETFL, flags | O_NONBLOCK) < 0)
            throw Server::exception(RED "Error: fcntl failed" RESET);

        if (bind(servers[i]._socketFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
            if (errno == EADDRINUSE)
                throw Server::exception(RED "Error: <bind> port is in use by other server" RESET);
        }
        if (listen(servers[i]._socketFd, 10) < 0)
            throw Server::exception(RED "Error: listen failed" RESET);
    }
}

static void acceptNewConnection(int serverSocket, vector<struct pollfd>& pollFds)
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

static void readRequest(vector<struct pollfd>& pollFds, int i, map<int, Request> requests)
{
    char buffer[65535];
    ssize_t bytesReceived = recv(pollFds[i].fd, buffer, sizeof(buffer), 0);
    if (bytesReceived > 0) {
        requests[pollFds[i].fd] = Request(buffer);
    } else if (bytesReceived == 0) {
        cout << "Connection closed" << endl;
        requests.erase(pollFds[i].fd);
    } else {
        perror("Error: recv failed");
        requests.erase(pollFds[i].fd);
    }

    close(pollFds[i].fd);
    pollFds.erase(pollFds.begin() + i);
}

static void sendResponse(vector<struct pollfd>& pollFds, int i, map<int, Request> requests)
{
    string hello = "HTTP/1.1 200/OK\r\nConnection: keep-alive\r\nHello from server";

    send(pollFds[i].fd, hello.c_str(), hello.size(), 0);
    cout << "Message sent" << endl;

    requests.erase(pollFds[i].fd);
    close(pollFds[i].fd);
    pollFds.erase(pollFds.begin() + i);
}

void Server::setupPolls(vector<Server> servers)
{
    int returnValue;
    vector<struct pollfd> pollFds(servers.size());
    map<int, Request> requests;

    for (size_t i = 0; i < servers.size(); i++) {
        pollFds[i].fd = servers[i]._socketFd;
        pollFds[i].events = POLLIN | POLLOUT;
    }
    while (true) {
        returnValue = poll(pollFds.data(), pollFds.size(), 60 * 1000);
        switch (returnValue) {
            case 0:
                cout << "Error: poll Timeout" << endl;
                break;

            case -1:
                cout << "Error: poll failed" << endl;
                break;

            default:
                for (size_t i = 0; i < pollFds.size(); i++) {
                    if ((pollFds[i].revents & POLLIN) && (i < servers.size())) {
                        acceptNewConnection(pollFds[i].fd, pollFds);
                    } else if (pollFds[i].revents & POLLIN) {
                        readRequest(pollFds, i, requests);
                    } else if (pollFds[i].revents & POLLOUT && (requests.find(pollFds[i].fd) != requests.end()
                        && requests[pollFds[i].fd].isReadyForResponse())) {
                        sendResponse(pollFds, i, requests);
                    }
                }
                break;
        }
    }
}