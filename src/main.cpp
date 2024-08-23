#include <sys/poll.h>
#include "class/Run/Run.hpp"
#include "class/Server/Server.hpp"
#include "../include/Utils.hpp"

vector<Server> signServers;
vector<struct pollfd> signPollFds;

void	handleSignals(int sigNum)
{
	(void)sigNum;
	if (!signServers.empty()){
		for (size_t i = 0; i < signServers.size(); i++)
			signServers[i].getRoute().clear();
	}
	cerr << YELLOW << "\nBye! 👋" << RESET << endl;
	exit (0);
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
        run.startServer(signServers, signPollFds);
    }
    catch (const Server::exception& e) {
        cerr << e.what() << '\n';
    }
    return 0;
}