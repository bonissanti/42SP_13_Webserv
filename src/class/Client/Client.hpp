#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../Request/Request.hpp"
#include "../Response/Response.hpp"
#include "../../../include/defines.hpp"

class Client {
    private:
        int _fd;
        Request _request;

    public:
        Client(int socket);
        Client();
        ~Client();
        void setFd(int newFd);
        void setRequest(Request newRequest);



    class exception : public std::exception {
        private:
            string msg;

        public:
            exception(const string& msg);
            virtual ~exception() throw();
            virtual const char* what() const throw();
    };
};

#endif  // CLIENT_HPP
