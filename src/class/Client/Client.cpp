#include "../Client/Client.hpp"

Client::Client(){

}

Client::~Client(){

}

void Client::sendResponse(Request client)
{
    // para teste
    string hello =
        "HTTP/1.1 200/OK\r\n"
        "Content-type: text-plain\r\n"
        "Content-Length: 17\r\n"
        "Connection: close\r\n"
        "\r\n"
        "Hello from server";

    // if (client.method_ == "GET")
    //     handleGet();
    // else if () POST e outro para DELETE depois

    send(_socket, hello.c_str(), hello.size(), 0);
    cout << "Message sent" << endl;

    client.getRequest().erase(_socket);
    close(_socket);
}

// Request receiveRequest()
// {
//     readRequest();
// }