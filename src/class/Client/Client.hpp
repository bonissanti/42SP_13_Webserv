#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <exception>

#include "../Request/Request.hpp"
#include "../Response/Response.hpp"
#include "../Server/Server.hpp"
#include "../Utils/Utils.hpp"

class Client {
    private:
        Server _server;
        Response _response;
        Request _request;
        string _executor;
        map<string, string> _mimeTypes;
        static bool _subdirAutoindex;
        map<int, Server*> _fdsMap;

        void freeEnviron(char** envp);
        bool checkFile(const string& file);
        string readCGI(const string& tempFile);
        int getMethodIndex(Route &route, string method);
        string defineFilePath(Route &route, string uri);
        string defineContentType(string filePath);
        string defineResponseBody(const Route &route, const string &filePath);
        bool verifyPermission(const string &file);
        string defineContentLength(const string &body);
        void setResponseData(int statusCode, string filePath, string contentType, string responseBody, string location);
        char** configEnviron(Server& server, Request& req);
        string executeCGI(Request& req, Server &server, string filePath);
        string setPageError(int errorCode, const string& filePath);
        Route findMatchingRoute(string uri, bool &subdirAutoindex);

    public:
        Client();
        ~Client();
        void addAssociation(int clientFd, Server& server);
        int callMethod(void);
        int runGetMethod(string filePath, Route matchedRoute);
        int runPostMethod(string uri);
        int runDeleteMethod(string filePath);

        void handleMultiPartRequest(void);
        bool saveUploadedFile(const string &filename, const std::vector<char> &fileContent, const string &directory);
        void sendResponse(struct pollfd& pollFds, map<int, Request>& requests);
        int getMethodIndex(string method);
        Server* getServerFd(int clientFd);
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