#include <sys/poll.h>

#include "class/Run/Run.hpp"
#include "class/Server/Server.hpp"
#include "class/Utils/Utils.hpp"

int main(int argc, char** argv)
{
    try {
        if (argc != 2)
            throw Server::exception(RED "Error: invalid number of arguments" RESET);

        int numbersOfServers = Run::setServersNumber(argv[1]);
        vector<Server> servers(numbersOfServers);

        ifstream file(argv[1]);
        if (!file.is_open()) {
            return -1;
        }

        for (int i = 0; i < numbersOfServers; i++) {
            servers[i].create(file);
            servers[i].openPortsToListen();
        }
        Run::startServer(servers);
    }
    catch (const Server::exception& e) {
        cerr << e.what() << '\n';
    }
    return 0;
}