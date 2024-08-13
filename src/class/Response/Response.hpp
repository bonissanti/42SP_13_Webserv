#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <map>
#include <string>
#include "../Request/Request.hpp"
#include "../../../include/defines.hpp"

class Response {
    private:
        int 				_statusCode;
        string				_index;
        string				_executor;
        string				_statusMessage;
        string 				_body;
        string				_filePath;
        string				_contentType;
        size_t				_contentLength;
        map<string, string> _headers;

        // string getStatusMessage(int code) const;

    public:
        Response(Request& req);
		~Response();

        int getMethodIndex(string method);
        void callMethod(Request& req);
        void runGetMethod(Request& req);
        string	executeCGI(Request& req);
        string	setResponseBody(Request& req); 
        char **configEnviron(Request& req);
        
        // void setStatusCode(int code);
        // void setBody(const string& body);
        // void setHeader(const string& field, const string& value);

        static void sendResponse(struct pollfd& pollFds, map<int, Request>& requests);

};

#endif  // RESPONSE_HPP
