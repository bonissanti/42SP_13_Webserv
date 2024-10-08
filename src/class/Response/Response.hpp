#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "../Utils/Utils.hpp"
#include "../Request/Request.hpp"

class Response {
    private:
        int _statusCode;
        string _index;
        string _statusMessage;
        string _responseBody;
        string _filePath;
        string _contentType;
        string _contentLength;
        string _location;
        map<string, string> _headers;

    public:
        Response();
        ~Response();

        int getMethodIndex(string method);
        string handleAutoIndex(string filePath, const string& uri);
        string defineResponseBody(const string& filePath, Request& req);
        string defineResponseBody(Request& req);
        string defineContentLength(const string& body);

        void setStatusCode(int code);
        void setFilePath(string filePath);
        void setContentType(string contentType);
        void setContentLength(string length);
        void setHeaders(map<string, string> headers);
        void setHeader(const string& field, const string& value);
        void setResponseBody(string responseBody);
        void setStatusMessage(string statusMessage);
        void setIndex(string index);
        void setLocation(string location);

        string getFilePath(void) const;
        string getBody(void) const;
        string getIndex(void) const;
        string getLocation(void) const;
        int getStatusCode(void) const;

        string setCreatedBody(const string& filePath);
		string getStatusPage(int statusCode);
        string buildMessage(void);
        void clear();
};

#endif  // RESPONSE_HPP
