#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <map>
#include "defines.hpp"

class Request {
    public:
        Request(const string &raw_request);
        string getMethod() const;
        string getPath() const;
        string getHeader(const string &field) const;
        string getBody() const;

    private:
        void parseRequest(const string &raw_request);

        string method_;
        string path_;
        map<string, string> headers_;
        string body_;
};

#endif  // REQUEST_HPP
