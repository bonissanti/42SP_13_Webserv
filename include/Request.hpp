#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <map>
#include "defines.hpp"

class Request {
    public:
        Request(const string &raw_request);
        ~Request();
	Request();

        void    appendMessage(const string &raw_request);
        
        map<string, string>  getHeaders() const;
        string  getMethod() const;
        string  getPath() const;
	string  getVersion() const;
        string  getHeader(const string &field) const;
        string  getBody() const;
        bool    getIsCgi() const;
        int     getStatusCode();

        void    printRequest() const;
        bool    validateRequest() const;
        void    parseRequest(const string &raw_request);

        void    isCgiRequest();
        bool    isReadyForResponse() const;
        void    setReadyForResponse(bool ready);

    private:
	bool    validateMethod() const;
        bool    validateHeaders() const;
        bool    validateVersion() const;


        void    parseRequestLine(const string &line);
        void    parseHeaders(istringstream &request_stream);
        void    parseBody(istringstream &request_stream);

        string  generateErrorResponse(int statusCode) const;

        map<string, string> headers_;
        string  method_;
        string  path_;
        string  version_;
        string  body_;
        string  buffer_;
        bool    isCgi_;
        bool    readyForResponse;
        int     statusCode_;

};

#endif  // REQUEST_HPP
