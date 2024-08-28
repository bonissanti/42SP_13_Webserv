#ifndef RUN_HPP
#define RUN_HPP

#include "../Utils/Utils.hpp"
#include "../Server/Server.hpp"

class Run {
    private:
        int _serversNumber;
        vector<Server> servers;

    public:
        Run(void);
        ~Run();
        void setServersNumber(string filePath);
        void startServer(vector<Server>& servers);
        int acceptNewConnection(int serverSocket, vector<struct pollfd>& pollFds);

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
