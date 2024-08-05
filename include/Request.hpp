#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <map>
#include "defines.hpp"

class Request {
    public:
        Request(const string &raw_request);
        string	getMethod() const;
        string	getPath() const;
        string	getHeader(const string &field) const;
        string	getBody() const;
        static void	executeCGI(void);
        static void	readRequest(vector<struct pollfd>& pollFds, int i);
        void	parseRequest(char buffer[], ssize_t bytesReceived);
        
    private:
        string _method;
        string _path;
        string _protocol;
        map<string, string> _headers;
        string _body;
};

#endif  // REQUEST_HPP
