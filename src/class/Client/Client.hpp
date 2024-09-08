#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <exception>

#include "../Request/Request.hpp"
#include "../Response/Response.hpp"
#include "../Server/Server.hpp"
#include "../Utils/Utils.hpp"

class Client {
    private:
        Server *_server;
        Response *_response;
        Request *_request;
        map<string, string> _mimeTypes;
        static bool _subdirAutoindex;

        string defineFilePath(Route &route, string uri);
        string defineContentType(string filePath);
        string defineResponseBody(const Route &route, const string &filePath, const string &uri);
        bool verifyPermission(const string &file);
        string defineContentLength(const string &body);
        void setResponseData(int statusCode, string filePath, string contentType, string responseBody, string location);

    public:
        Client();
        ~Client();
        void addAssociation(int clientFd, Server server);
        int callMethod(void);
        int runGetMethod(void);
        int runPostMethod(void);
        int runDeleteMethod(void);
        void handleMultiPartRequest(void);
        bool saveUploadedFile(const string &filename, const std::vector<char> &fileContent, const string &directory);
        void sendResponse(void);
        int getMethodIndex(string method);
        Server* getServer(void);
        Response *getResponse(void);
        Request *getRequest(void);
        void setResponseData(int statusCode, string filePath, string contentType, string responseBody);
        void setServer(Server& server);

        class ClientException : public std::exception {
            private:
                string msg;

            public:
                ClientException(const string &message);
                virtual ~ClientException() throw();
                virtual const char *what() const throw();
        };
};

#endif