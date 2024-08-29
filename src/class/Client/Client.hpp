#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <exception>

#include "../Utils/Utils.hpp"
#include "../Request/Request.hpp"
#include "../Response/Response.hpp"
#include "../Server/Server.hpp"

class Client {
    private:
        int _clientFd;
        Server _server;
        Response _response;
        Request _request;

        string defineFilePath(void);
        string defineContentType(string filePath);
        string defineResponseBody(const string &filePath, Request &req);
        bool verifyPermission(const string &file);
        string defineContentLength(const string &body);
    public:
        Client();
        ~Client();
        void addAssociation(int clientFd, Server server);
        Server getServerFd(int clientFd);
        int callMethod(void);
        int getMethodIndex(string method);
        int runGetMethod(void);
        int runPostMethod(void);
        int runDeleteMethod(void);
        bool saveUploadedFile(const string& filename, const string& fileContent, const string& directory);
        void sendResponse(struct pollfd& pollFds, map<int, Request>& requests);

        class ClientException : public std::exception {
            private:
                string msg;

            public:
                ClientException(const string& message);
                virtual ~ClientException() throw();
                virtual const char* what() const throw();
        };
};

#endif