#include "../include/Server.hpp"
#include "../include/Utils.hpp"
#include "../include/Validate.hpp"
using namespace std;

int main(int argc, char** argv)
{
    try {
        (void)argc;
        if (argc != 2)
            throw Validate::exception(RED "Error: invalid number of arguments" RESET);

        int numbersOfServers = Utils::getServersNumber(argv[1]);

        if (numbersOfServers == -1)
            throw Validate::exception(RED "Error: invalid config file" RESET);
        else if (numbersOfServers == 0)
            throw Validate::exception(RED "Error: empty config file" RESET);

        Server servers[numbersOfServers];
        for(int i = 0; i < numbersOfServers; i++)
        {
            servers[i].
        }

        // Validate request(argv[1]);
        // std::vector<Server> servers = Server::creatingServers(request.getNumServers(), request.getLines());

        // Server::startServer(servers); //cria o socket, verifica se a porta está em uso com setsockopt, chama bind e
        // listen (se ok)
    }
    catch (const Validate::exception& e) {
        cerr << e.what() << '\n';
    }
}