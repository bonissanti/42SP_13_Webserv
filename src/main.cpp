#include "class/Run/Run.hpp"
#include "class/Server/Server.hpp"
#include "class/Utils/Utils.hpp"

vector<Server> signServers;
bool signalUsed = false;

void	handleSignals(int sigNum)
{
	(void)sigNum;
	cerr << YELLOW << "\nBye! 👋" << RESET << endl;
    signalUsed = true;
}

int main(int argc, char** argv)
{
    Run run;
    try {
        if (argc != 2)
            throw Server::exception(RED "Error: invalid number of arguments" RESET);

        int numbersOfServers = Utils::getServersNumber(argv[1]);
        for (int i = 0; i < numbersOfServers; i++)
        	signServers.push_back(Server());
        
        ifstream file(argv[1]);
        if (!file.is_open()) {
        	return -1;
        }
        
        for (int i = 0; i < numbersOfServers; i++) {
            signServers[i].create(file);
        }
        file.close();
        signal(SIGINT, handleSignals);
        Server::configServer(signServers);
        run.startServer(signServers);
    }
    catch (const std::exception& e) {
        cerr << e.what() << '\n';
    }
    return 0;
}