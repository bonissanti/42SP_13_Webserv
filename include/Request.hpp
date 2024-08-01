#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <map>
#include "defines.hpp"

class Request {
    public:
        Request(const string &raw_request);
        ~Request();
		Request();

        string  getMethod() const;
        string  getPath() const;
		string  getVersion() const;
        string  getHeader(const string &field) const;
        string  getBody() const;
        void    printRequest() const;
        bool    validateRequest(string& errorResponse) const;

    private:

		bool    validateMethod() const;
        bool    validateHeaders() const;
        bool    validateVersion() const;

        void    parseRequest(const string &raw_request);

        string  method_;
        string  path_;
        string  version_;
        map<string, string> headers_;
        string  body_;
};

void handle_request(int client_socket);

#endif  // REQUEST_HPP
