#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <exception>

#include "../../../include/defines.hpp"
#include "../Request/Request.hpp"
#include "../Response/Response.hpp"
#include "../Server/Server.hpp"

class Client {
    private:
        map<int, Server> _fdsMap;
        map<string, string> _mimeTypes;
        Response _response;
        Request _request;

        string defineFilePath(string uri);
        string defineContentType(string filePath);
        string defineResponseBody(const string& filePath, const string& uri);
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
        bool saveUploadedFile(const string& filename, const std::vector<char>& fileContent, const string& directory);
        void sendResponse(struct pollfd& pollFds, map<int, Request>& requests);
        void setResponseData(int statusCode, string filePath, string contentType, string responseBody);

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