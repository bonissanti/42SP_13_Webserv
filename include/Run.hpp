#ifndef RUN_HPP
# define RUN_HPP

#include "defines.hpp"
#include "Server.hpp"

class Run{

	private:
	vector<Server> servers;

	public:
	vector<struct pollfd> loadPolls(vector<Server> servers);
	void startServer(vector<Server>& servers);
};

#endif