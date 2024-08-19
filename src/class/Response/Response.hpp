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
        map<string, string> parseMultipartData(const string& body, const string& boundary);
        void saveUploadedFile(const string& filename, const string& fileContent, const string& directory);
        map<string, string> parseUrlEncodedData(const string& body);

    public:
        Response();
        ~Response();

        int getMethodIndex(string method);
        void runGetMethod(Request& req);
        void runPostMethod(Request& req);
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

        string getFilePath(void) const;
        void setHeader(const string& field, const string& value);
};

#endif  // RESPONSE_HPP
