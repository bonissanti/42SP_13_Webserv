#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../Request/Request.hpp"
#include "../Response/Response.hpp"
#include "../../../include/defines.hpp"

class Client {
    public:
        Client(int socket);
        Client();
        ~Client();
        Request receiveRequest();
        void sendResponse(Request client);

    private:
        int _socket;

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
