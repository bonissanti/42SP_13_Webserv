#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <map>
#include "defines.hpp"

class Request {
    public:
        Request(const string &raw_request);
        ~Request();
		Request(); //remover apos testes

        string getMethod() const;
        string getPath() const;
		string getVersion() const;
        string getHeader(const string &field) const;
        string getBody() const;
        void parseRequest(const string &raw_request); //mover para private depois dos testes
        void printRequest() const;

    private:
		bool validateFields() const;

        string method_;
        string path_;
        string version_;
        map<string, string> headers_;
        string body_;
};

#endif  // REQUEST_HPP
