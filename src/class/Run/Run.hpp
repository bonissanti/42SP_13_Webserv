#ifndef RUN_HPP
#define RUN_HPP

#include "../../../include/Utils.hpp"
#include "../../../include/defines.hpp"
#include "../Server/Server.hpp"

class Run {
    private:
		vector<Server> servers;
    public:
        Run(void);
        ~Run();
        vector<struct pollfd> loadPolls(vector<Server> servers);
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
