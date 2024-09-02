#include "Server.hpp"

Server::Server()
{
    _listen = 100;
    _client_max_body_size = 2 * MB;
    _server_name = "default";
    _root = "/data/";
}

Server::Server(const Server& toCopy){
    _listen = toCopy._listen;
    _socketFd = toCopy._socketFd;
    _fd = toCopy._fd;
    _server_name = toCopy._server_name;
    _root = toCopy._root;
    _client_max_body_size = toCopy._client_max_body_size;
    _error_page = toCopy._error_page;
    _routes = toCopy._routes;
}

Server::~Server() {}

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
            try
            {
                Route new_route;
                new_route.create(line, file);
                _routes.push_back(new_route);
                routeFound = true;
            }
            catch(const Route::exception& e){
                throw;
            }
        }
    }
    if (_listen == 100)
        throw Server::exception(RED "Error: server or listen are not set" RESET);
}

void Server::openPortsToListen(void)
{
    int inUse = 1;
    struct sockaddr_in serverAddr;

    Utils::bzero(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    serverAddr.sin_port = htons(_listen);
    _socketFd = socket(AF_INET, SOCK_STREAM, 0);

    if (setsockopt(_socketFd, SOL_SOCKET, SO_REUSEADDR, &inUse, sizeof(int)) == -1)
        throw Server::exception(RED "Error: setsockopt failed" RESET);



    if (bind(_socketFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        if (errno == EADDRINUSE)
            throw Server::exception(RED "Error: <bind> port is in use by other server" RESET);
    }
    if (listen(_socketFd, 10) < 0)
        throw Server::exception(RED "Error: listen failed" RESET);

    // set pollfd
    _fd.fd = _socketFd;
    _fd.events = POLLIN | POLLOUT;
    _fd.revents = 0;
}

Server::Server::exception::exception(const string& msg) : msg(msg) {}
Server::Server::exception::~exception() throw() {}

const char* Server::Server::exception::what() const throw()
{
    return (this->msg.c_str());
}

string setRoot(string root)
{
    if (root[0] != '/')
        throw Server::exception(RED "Error: misformatted root path, please use '/path'" RESET);
    if (root[root.length() - 1] != '/')
        root.insert(root.end(), '/');
    return (root);
}

