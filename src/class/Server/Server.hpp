#ifndef SERVER_HPP
#define SERVER_HPP

#include "../../../include/Config.hpp"
#include "../../../include/defines.hpp"
#include "../../../include/Utils.hpp"
#include "../../../include/Utils.hpp"
#include "../Request/Request.hpp"
#include "../Response/Response.hpp"
#include "../Route/Route.hpp"



class Server {
    private:
		int _listen;
        int _socketFd;
        int _pollFd;
        string _server_name;
        string _root;
        int _client_max_body_size;
        vector<map<int, string> > _error_page;
        vector<Route> _routes;

    public:
        Server(void);
        ~Server();

        int getSocket(void);
        
        void create(ifstream& file);
        void setServerName(string name);
        void setClientMaxBodySize(string size);
        void setErrorPage(string error_page);
        void setListen(int port);
        static void configServer(vector<Server>& servers);

        void setRoute(vector<string> routeLines, size_t& i);
        vector<struct pollfd> loadPolls(vector<Server> servers);

        void startServer(vector<Server> servers);
        static void setupPolls(vector<Server> servers);

        string getRoot() const;

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
