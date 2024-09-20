#ifndef RUN_HPP
#define RUN_HPP

#include "../Server/Server.hpp"
#include "../Utils/Utils.hpp"
#include "../Client/Client.hpp"
#include <stack>

class Run {
    private:
        map<int, Client*> _mapClient;

    public:
        Run(void);
        ~Run();
    	Client* getClientFd(int clientFd);
        int setServersNumber(string filePath);
        void startServer(vector<Server>& servers);

        class exception : public std::exception {
            private:
                string msg;

            public:
                exception(const string& msg);
                virtual ~exception() throw();
                virtual const char* what() const throw();
        };
};

#endif  // SERVER_HPP