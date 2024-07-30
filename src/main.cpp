#include "../include/Server.hpp"
#include "../include/Utils.hpp"
#include "../include/Validate.hpp"

int main(int argc, char** argv)
{
    try {
        (void)argc;
        if (argc != 2)
            throw Validate::exception(RED "Error: invalid number of arguments" RESET);

        int numbersOfServers = Utils::getServersNumber(argv[1]);

        vector<Server> servers(numbersOfServers);

        ifstream file(argv[1]);
        if (!file.is_open()) {
            return -1;
        }

        for (int i = 0; i < numbersOfServers; i++) {
            servers[i].create(file);
        }
        
        while (1) {
            
        }
    }
    catch (const Validate::exception& e) {
        cerr << e.what() << '\n';
    }
}