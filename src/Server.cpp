#include "../include/Server.hpp"

#include "../include/Route.hpp"

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
    string line;
    while (getline(file, line)) {
        line = Utils::trim(line);
        if (line.empty() or line[0] == '#') {
            continue;
        }

        if (line.substr(0, 2) == "}," and line.length() == 2)
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
        }
    }

    if (_listen == 100)
        throw Server::exception(RED "Error: listen is not set" RESET);
    if (file.good()) {
        file.seekg(-line.length(), ios_base::cur);
    }
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