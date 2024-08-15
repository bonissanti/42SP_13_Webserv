#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <map>
#include <string>
#include "../Request/Request.hpp"
#include "../../../include/defines.hpp"



class Response {
    private:
        int _statusCode;
        string _statusMessage;
        map<string, string> _headers;
        string _body;

        // string getStatusMessage(int code) const;
        map<string, string> parseMultipartData(const string& body, const string& boundary);
        void saveUploadedFile(const string& filename, const string& fileContent, const string& directory);
        map<string, string> parseUrlEncodedData(const string& body);

    public:
        Response(Request& req);
		~Response();

        int getMethodIndex(string method);
        void callMethod(Request& req);
        void runGetMethod(Request& req);

        void runPostMethod(Request& req);
        void setStatusCode(int code);
        void setBody(const string& body);
        void setHeader(const string& field, const string& value);
        void setHeaders(map<string, string>);
        string toString() const;

        int getStatusCode() const {
            return _statusCode;
        }

        string getStatusMessage(int code) const;

        const map<string, string>& getHeaders() const {
            return _headers;
        }

        string getBody() const {
            return _body;
        }
        string getResponse() const;

        static void sendResponse(vector<struct pollfd>& pollFds, int i, map<int, Request>& requests);

};

#endif  // RESPONSE_HPP
