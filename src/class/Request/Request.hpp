#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <cstddef>
#include <map>
#include <string>
#include "../../../include/defines.hpp"
#include "../Server/Server.hpp"

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
        static void	readRequest(vector<struct pollfd>& pollFds, int i, map<int, Request>& requests);
        friend class Response;

    private:
        void    parseRequestLine(const string &line);
        void    parseHeaders(istringstream &request_stream);
        void    parseBody(istringstream &request_stream);
        string  generateErrorResponse(int statusCode) const;
        
        map<int, Request> _requests; 
        map<string, string> _headers;
        string  	_method;
        string  	_path;
        string  	_version;
        string  	_body;
        bool    	_isCgi;
        HttpStatus  _statusCode;
        Server		_svConnection;

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
