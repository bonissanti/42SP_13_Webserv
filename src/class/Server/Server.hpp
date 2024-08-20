#ifndef SERVER_HPP
#define SERVER_HPP

#include "../../../include/Config.hpp"
#include "../../../include/defines.hpp"
#include "../../../include/Utils.hpp"
#include "../../../include/Utils.hpp"
#include "../Route/Route.hpp"

class Server {
    private:
		int _listen;
        int _socketFd; //socket()
        int _pollFd;
        string _server_name;
        string _root;
        int _client_max_body_size;
        vector<map<int, string> > _error_page;
        vector<Route> _routes;
        // vector<int> _clientFd;

    public:
        Server(void);
        ~Server();

        int getSocket(void);
        int getClientFd(void);
        int getListen(void);
        string getServerName(void);
        void addClient(int clientFd); 
        
        void create(ifstream& file);
        void setServerName(string name);
        void setClientMaxBodySize(string size);
        void setErrorPage(string error_page);
        void setListen(int port);
        void setClientFd(int clientFd);
        void setRoute(vector<string> routeLines, size_t& i);
        
        vector<Route> getRoute(void); 
        
        static void configServer(vector<Server>& servers);
        vector<struct pollfd> loadPolls(vector<Server> servers);

        void startServer(vector<Server> servers);
        static void setupPolls(vector<Server> servers);

        class exception : public std::exception {
            private:
                string msg;

            public:
                exception(const string& msg);
                virtual ~exception() throw();
                virtual const char* what() const throw();
        };
};

string setRoot(string root);

#endif  // SERVER_HPP
