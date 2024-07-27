#include "../include/Server.hpp"

static int getKeyIndex(string key);

Server::Server()
{
    status_request status = DEFAULT;

    _listen = 8080;
    _client_max_body_size = 2 * MB;
    _server_name = "default";
    _root = "/data/";

    _error_page.push_back(status);
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
        if (line.empty()) {
            continue;
        }

        if (line.find("server") == 0)
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
            Route route;
            // create a route and push it on the vector
            _routes.push_back(route);
        }
    }

    if (file.good()) {
        file.seekg(-line.length(), ios_base::cur);
    }
}

static int getKeyIndex(string key)
{
    string types[] = {"listen", "server_name", "host", "root", "client_max_body_size"};

    for (size_t i = 0; i < 6; i++)
        if (key == types[i])
            return (i);
    return (-1);
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
    _error_page.push_back(Utils::strtoi(error_page));
}

// static int checkInsideRoute(string config)
// {
//     string type[] = {"autoindex", "root", "allow_methods", "index", "cgi", "_redirect"};  // adicionar outros

//     for (int i = 0; i < 6; i++)
//         if (config == type[i])
//             return (i);
//     return (-1);
// }

// void Server::setRoute(vector<string> lines, size_t& i)
// {
//     static int routeIndex;

//     _route.push_back(createRoute());

//     for (; i < lines.size(); i++) {
//         string key, value;
//         stringstream ss(lines[i]);

//         if (getline(ss, key, '=') && getline(ss, value)) {
//             switch (checkInsideRoute(trim(key))) {
//                 case AUTOINDEX:
//                     _route[routeIndex]._autoIndex = (trim(value) == "on") ? true : false;
//                     break;

//                 case RROOT:
//                     _route[routeIndex]._root = setRoot(trim(value));
//                     break;

//                 case AMETHODS:
//                     _route[routeIndex]._allowMethods = setMethods(trim(value));
//                     break;

//                 case INDEX:
//                     _route[routeIndex]._index = setIndex(trim(value));
//                     break;

//                 case CGI:
//                     _route[routeIndex]._cgi = setCGI(trim(value));
//                     break;

//                 case REDIRECT:
//                     _route[routeIndex]._redirect = setRedirect(trim(value));
//                     break;
//             }
//         }
//         else if (lines[i] == "}")
//             break;
//     }
//     ++routeIndex;
// }