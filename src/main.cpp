#include "../include/Server.hpp"
#include "../include/Utils.hpp"

int main(int argc, char** argv)
{
    try {
        if (argc != 2)
            throw Server::exception(RED "Error: invalid number of arguments" RESET);

        int numbersOfServers = Utils::getServersNumber(argv[1]);
        vector<Server> servers(numbersOfServers);

        ifstream file(argv[1]);
        if (!file.is_open()) {
            return -1;
        }

        for (int i = 0; i < numbersOfServers; i++) {
            servers[i].create(file);
        }
        Server::startServer(servers); // loadConfig
        Server::setupPolls(servers);
    }
    catch (const Server::exception& e) {
        cerr << e.what() << '\n';
    }
    return 0;
}