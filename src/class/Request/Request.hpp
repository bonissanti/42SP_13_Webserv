#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <map>
#include "../../../include/defines.hpp"


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
        bool    getIsCgi() const;
        int     getStatusCode() const;
        map<int, Request> getRequest() const;
        
        void    printRequest() const;
        bool    validateRequest() const;
        static void	handleCGI(void);
        static void	executeCGI(void);
        void	readRequest(vector<struct pollfd>& pollFds, int i);
        bool    isRequestComplete(const std::string& request);
        void    parseRequest(const string &raw_request);
        void    isCgiRequest();

    private:
        void    parseRequestLine(const string &line);
        void    parseHeaders(istringstream &request_stream);
        void    parseBody(istringstream &request_stream);
        string  generateErrorResponse(int statusCode) const;
        
        map<int, Request> requests_; 
        map<string, string> headers_;
        string  method_;
        string  path_;
        string  version_;
        string  body_;
        bool    isCgi_;
        int     statusCode_;

    class exception : public std::exception {
        private:
            string msg;

        public:
            exception(const string& msg);
            virtual ~exception() throw();
            virtual const char* what() const throw();
    };
};

#endif  // REQUEST_HPP
