#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <map>
#include <string>

#include "defines.hpp"

class Response {
    private:
        int _statusCode;
        string _statusMessage;
        map<string, string> _headers;
        string _body;

        string getStatusMessage(int code) const;

    public:
        Response();
        void setStatusCode(int code);
        void setBody(const string& body);
        void setHeader(const string& field, const string& value);
        string toString() const;
};

#endif  // RESPONSE_HPP
