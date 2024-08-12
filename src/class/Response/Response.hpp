#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <map>
#include <string>
#include "../Request/Request.hpp"
#include "../../../include/defines.hpp"



class Response {
    private:
        int _statusCode;
        string _statusMessage;
        map<string, string> _headers;
        string _body;

        // string getStatusMessage(int code) const;

    public:
        Response(Request& req);
		~Response();

        int getMethodIndex(string method);
        void callMethod(Request& req);
        void runGetMethod(Request& req);
        // void setStatusCode(int code);
        // void setBody(const string& body);
        // void setHeader(const string& field, const string& value);
        // string toString() const;

        static void sendResponse(vector<struct pollfd>& pollFds, int i, map<int, Request>& requests);

};

#endif  // RESPONSE_HPP
