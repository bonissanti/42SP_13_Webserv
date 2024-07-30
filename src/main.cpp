#include "../include/Server.hpp"
#include "../include/Utils.hpp"

int main(int argc, char** argv)
{
    try {
        if (argc != 2)
            throw Server::exception(RED "Error: invalid number of arguments" RESET);

        int numbersOfServers = Utils::getServersNumber(argv[1]);
        Server servers[numbersOfServers];
        for(int i = 0; i < numbersOfServers; i++)
        {
            servers[i].
        }

        // Validate request(argv[1]);
        // std::vector<Server> servers = Server::creatingServers(request.getNumServers(), request.getLines());

        Server::startServer(servers);
        Server::setupPolls(servers);
    }
    catch (const Server::exception& e) {
        std::cerr << e.what() << '\n';
        vector<Server> servers(numbersOfServers);

        ifstream file(argv[1]);
        if (!file.is_open()) {
            return -1;
        }

        for (int i = 0; i < numbersOfServers; i++) {
            servers[i].create(file);
        }
    }
    catch (const Server::exception& e) {
        cerr << e.what() << '\n';
    }
    return 0;
}