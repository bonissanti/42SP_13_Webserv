#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Request.hpp"
#include "Response.hpp"

class Client {
    public:
        Client(int socket);
        Request receiveRequest();
        void    sendResponse(const Response& response);

    private:
        int socket_;
};

#endif  // CLIENT_HPP
