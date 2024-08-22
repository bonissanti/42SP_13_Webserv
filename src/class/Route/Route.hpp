#ifndef ROUTE_HPP
#define ROUTE_HPP

#include "../../../include/Utils.hpp"
#include "../../../include/defines.hpp"

class Route {
    private:
        string _route;
        bool _autoIndex;
        bool _cgiOn;
        string _root;
        string _allowMethods;
        string _index;
        string _redirect;
        string _return;
        string _cgi;

    public:
        Route();
        ~Route();

        void create(const string& line, ifstream& file);
        bool getAutoIndex() const;
        bool getCgiOn() const;
        string getRoot() const;
        string getAllowMethods() const;
        string getIndex() const;
        string getRedirect() const;
        string getReturn() const;
        string getCgi() const;
        string getRoute() const;

        void setRoute(const string& route);
        void setAutoIndex(bool autoIndex);
        void setCgiOn(bool cgiOn);
        void setRoot(string& root);
        void setAllowMethods(const string& allowMethods);
        void setIndex(string& index);
        void setRedirect(const string& redirect);
        void setReturn(const string& ret);
        void setCgi(string& cgi);

        class exception : public std::exception {
            private:
                string msg;

            public:
                exception(const string& msg);
                virtual ~exception() throw();
                virtual const char* what() const throw();
        };
};

#endif  // ROUTE_HPP
