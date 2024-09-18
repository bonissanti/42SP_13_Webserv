#ifndef SERVER_HPP
#define SERVER_HPP

#include "../Route/Route.hpp"
#include "../Utils/Utils.hpp"

class Server {
    private:
        int _listen;
        int _socketFd;
        struct pollfd _fd;
        struct pollfd _tempFd;
        string _server_name;
        string _root;
        int _client_max_body_size;
        vector<map<int, string> > _error_page;
        vector<Route> _routes;
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
        struct pollfd& getPollFd(void);
        void getServerFd(void);

        void addClient(int clientFd);

        void create(ifstream& file);
        
        void setServerName(string name);
        void setClientMaxBodySize(string size);
        void setErrorPage(string error_page);
        void setListen(int port);
        void setClientFd(int clientFd);
        void setRoute(vector<string> routeLines, size_t& i);
        void setClientFd(struct pollfd& pollFd);
        void openPortsToListen(void);

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
