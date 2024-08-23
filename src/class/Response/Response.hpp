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
        string _contentLength;
        map<string, string> _headers;

        // string getStatusMessage(int code) const;
        void freeEnviron(char** envp);
        bool checkFile(const string& file);
        string readCGI(int fd);
    public:
        Response();
        ~Response();

        int getMethodIndex(string method);
        string executeCGI(Request& req, const string& filePath);
        string defineResponseBody(const string& filePath, Request& req);
        string defineResponseBody(Request& req);
        string defineContentLength(const string& body);
        char** configEnviron(Request& req);

        void setStatusCode(int code);
        void setFilePath(string filePath);
        void setContentType(string contentType);
        void setContentLength(string length);
        void setHeaders(map<string, string> headers);
        void setResponseBody(string responseBody);
        void setStatusMessage(string statusMessage);

        string getFilePath(void) const;
        string getBody(void) const;
        string getIndex(void) const;
        int getStatusCode(void) const;

        void setHeader(const string& field, const string& value);
        string buildMessage(void);

        void clear();
};

#endif  // RESPONSE_HPP
