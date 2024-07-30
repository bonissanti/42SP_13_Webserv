#ifndef SERVER_HPP
#define SERVER_HPP

#include "Config.hpp"
#include "Request.hpp"
#include "Utils.hpp"
#include "defines.hpp"
#include "Route.hpp"

typedef enum {
    DEFAULT = 0,
    OK = 200,
    BAD_REQUEST = 400,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    METHOD_NOT_ALLOWED = 405,
    INTERNAL_SERVER_ERROR = 500,
    BAD_GATEWAY = 502,
    MOVED_PERMANENTLY = 301,
} status_request;

class Server {
    private:
        int _listen;
        string _server_name;
        string _root;
        int _client_max_body_size;
        vector< map<int, string> > _error_page;
        vector<Route> _routes;

    public:
        Server(void);
        ~Server();

        void create(ifstream& file);
        static void startServer(vector<Server> servers);

        void setListen(int port);
        void setServerName(string name);
        void setHost(string host);
        void setClientMaxBodySize(string size);
        void setRoute(vector<string> routeLines, size_t& i);
        void setErrorPage(string error_page);

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
