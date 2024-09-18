#include "Server.hpp"

int Server::getSocket(void)
{
    return (_socketFd);
}

vector<Route> Server::getRoute(void)
{
    return (_routes);
}

string Server::getServerName(void)
{
    return (_server_name);
}

int Server::getListen(void)
{
    return (_listen);
}

// struct pollfd& Server::getPollFd(void)
// {
//     return (_fd);
// }

string Server::getRoot() const
{
    return _root;
}

// void Server::setClientFd(struct pollfd& pollFd)
// {
//     _tempFd = _fd;
//     _fd = pollFd;
// }

// void Server::getServerFd(void)
// {
//     _fd = _tempFd;
// }

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

int Server::getMaxBodySize() {
    return _client_max_body_size;
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
