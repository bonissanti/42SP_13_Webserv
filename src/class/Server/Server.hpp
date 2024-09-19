#ifndef SERVER_HPP
#define SERVER_HPP

#include "../Route/Route.hpp"
#include "../Utils/Utils.hpp"

class Server {
    private:
        int _listen;
        int _socketFd;
        string _server_name;
        string _root;
        int _client_max_body_size;
        vector<Route> _routes;
        map<int, string> _mapErrorPage;
        bool filterDuplicatesRoutes(Route& route);

    public:
        Server(void);
        Server(const Server& toCopy);
        ~Server();

        int getSocket(void);
        int getListen(void);
        string getServerName(void);
        vector<Route> getRoute(void);
        string getRoot() const;
        bool getErrorPage(int errorCode);
        void getServerFd(void);
        int getMaxBodySize();
        

        void addClient(int clientFd);

        void create(ifstream& file);
        
        void setServerName(string name);
        void setClientMaxBodySize(string size);
        void setErrorPage(string error_page);
        void setListen(int port);
        void setClientFd(int clientFd);
        void setRoute(vector<string> routeLines, size_t& i);
        void setClientFd(struct pollfd& pollFd);
        Route findMatchingRoute(const string& uri, bool& subdirAutoindex);
        static void configServer(vector<Server>& servers);
        void printErrorPages(void);

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
