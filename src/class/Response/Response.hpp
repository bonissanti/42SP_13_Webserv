#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <map>
#include <string>

#include "../../../include/defines.hpp"
#include "../Request/Request.hpp"

class Response {
    private:
        int _statusCode;
        string _index;
        string _executor;
        string _statusMessage;
        string _responseBody;
        string _filePath;
        string _contentType;
        size_t _contentLength;
        map<string, string> _headers;

        // string getStatusMessage(int code) const;
        void freeEnviron(char** envp);
        bool checkFile(const string& file);
        string getQueryString(string path);

    public:
        Response();
        ~Response();

        int getMethodIndex(string method);
        string executeCGI(Request& req);
        string defineFilePath(string& uri, Request& req); 
        string defineResponseBody(const string& filePath, Request& req); 
        string defineResponseBody(Request& req);
        string defineContentType(string filePath);
        size_t defineContentLength(const string& body);
        char **configEnviron(Request& req);

        void setStatusCode(int code);
        void setFilePath(string filePath);
        void setContentType(string contentType);
        void setContentLength(size_t length);
        void setHeaders(map<string, string> headers);
        void setResponseBody(string responseBody);
        void setStatusMessage(string statusMessage);

        string getFilePath(void) const;
        string getBody(void) const;
        void setHeader(const string& field, const string& value);
        string assembleResponse();
};

#endif  // RESPONSE_HPP
