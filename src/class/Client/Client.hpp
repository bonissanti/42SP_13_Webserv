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

        string defineFilePath(void);
        string defineContentType(string filePath);
        string defineResponseBody(const string &filePath);
        bool verifyPermission(const string &file);
        string defineContentLength(const string &body);

    public:
        Client();
        ~Client();
        void addAssociation(int clientFd, Server server);
        int callMethod(void);
        int runGetMethod(void);
        int runPostMethod(void);
        int runDeleteMethod(void);
        bool saveUploadedFile(const string &filename, const string &fileContent, const string &directory);
        void sendResponse(void);
        int getMethodIndex(string method);
        Server* getServer(void);
        Response *getResponse(void);
        Request *getRequest(void);
        void setServer(Server *server);

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