#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <map>
#include <string>

class Response {
    private:
        int _statusCode;
        string _statusMessage;
        map<string, string> _headers;
        string _body;

        // string getStatusMessage(int code) const;

    public:
        Response();
        void setStatusCode(int code);
        void setBody(const string& body);
        void setHeader(const string& field, const string& value);
        string toString() const;
        int status_code_;
        string status_message_;
        map<string, string> headers_;
        string body_;

        string getStatusMessage(int code) const;
};

#endif  // RESPONSE_HPP
