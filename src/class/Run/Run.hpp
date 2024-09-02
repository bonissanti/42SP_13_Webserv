#ifndef RUN_HPP
#define RUN_HPP

#include "../Server/Server.hpp"
#include "../Utils/Utils.hpp"
#include <stack>

class Run {
    private:
        Run(void);
        ~Run();
        static struct pollfd acceptNewConnection(int socketFd);

    public:
        static int setServersNumber(string filePath);
        static void startServer(vector<Server>& servers);

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
