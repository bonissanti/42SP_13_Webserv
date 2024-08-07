#include "../include/Client.hpp"

Client::Client(){

}

Client::~Client(){

}

void Client::sendResponse(pollfd & pollFd, Request client)
{
    string hello = "HTTP/1.1 200/OK\r\n\r\nHello from server";

    send(pollFd.fd, hello.c_str(), hello.size(), 0);
    cout << "Message sent" << endl;

    client.getRequest().erase(pollFd.fd);
    close(pollFd.fd);
}