#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Request.hpp"
#include "Response.hpp"
#include "defines.hpp"

class Client {
    public:
        Client(int socket);
        Client();
        ~Client();
        Request receiveRequest();
        void sendResponse(pollfd & pollFd, Request client);

    private:
        int socket_;
};

#endif  // CLIENT_HPP
