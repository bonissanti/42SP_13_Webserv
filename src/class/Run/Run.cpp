#include "Run.hpp"
#include <sys/poll.h>
#include "../Request/Request.hpp"
#include "../Response/Response.hpp"
#include "../Client/Client.hpp"

Run::Run(){}
Run::~Run(){}

/* function: loadPolls
-----------------------------
** @params: servers - Vector of servers passed by reference
** @atr: pollFds - Vector of servers and client sockets

** This function configures the pollFds vector of structs for use with the poll function. Each struct
** in pollFds represents a file descriptor and its associated events. The POLLIN flag indicates that 
** a file descriptor is ready for reading, and the POLLOUT flag indicates that it is ready for writing. 
** After calling the poll function, you can check the revents field of each struct in pollFds to determine
** which events occurred. For example:
** - If pollFds[i].revents & POLLIN is true, it means the file descriptor is ready to read.
** - If pollFds[i].revents & POLLOUT is true, it means the file descriptor is ready to write.
**
** @return: A vector of pollfd structs configured with file descriptors and event flags.
*/

vector<struct pollfd> Run::loadPolls(vector<Server> servers)
{
    vector<struct pollfd> pollFds(servers.size());

    for (size_t i = 0; i < servers.size(); i++) {
        pollFds[i].fd = servers[i].getSocket();
        pollFds[i].events = POLLIN | POLLOUT;
    }
    return (pollFds);
}

/* function: acceptNewConnection
-----------------------------
** @params: serverSocket
** @params: pollFds - only with server sockets
** @atr: clientFd - New socket for communication with the client
** @atr: socklen_t - A type alias for ssize_t, used to represent the size of socket address structures
** @atr: sockaddr_in - A struct used to work with web connections, containing address information

** This function is called when there is an incoming connection request on a server socket. 
** At this point, pollFds contains only server sockets, and no communication has been established yet.

** The accept() function is used to create a new communication channel with the client. It generates 
** a clientFd, which is the socket file descriptor used for communication between the client and server. 
** This new socket will be used by the client to send requests and by the server to respond. At the end 
** of the function, we configure the new socket similarly to how we configured the server sockets in 
** the loadPolls() function, so that poll() can monitor its status.

** @return: int as a file descriptor/socket
*/

int acceptNewConnection(int serverSocket, vector<struct pollfd>& pollFds)
{
    int clientFd;
    socklen_t addrlen;
    struct sockaddr_in clientAddr;

    addrlen = sizeof(clientAddr);
    clientFd = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrlen);
    if (clientFd == -1) {
        if (errno == EWOULDBLOCK)
            cout << "No pending connections for now" << endl;
        else
        	cerr << "Error: accept failed" << endl;
    }
    else
        cout << "New communication established!" << endl;  // log message

    struct pollfd commFd;
    
    commFd.fd = clientFd;
    commFd.events = POLLIN | POLLOUT;
    commFd.revents = 0;
    pollFds.push_back(commFd);
    return (clientFd);
}

/* function: startServer 
-----------------------------
** @params: servers - Vector of servers passed by reference
** @atr: returnValue - Used by poll to check if sockets are allowed to read or write
** @atr: requests - Map of all pending requests
** @atr: pollFds - Vector of servers and client sockets
** @atr: clientManager - Class used to associate requests with their respective servers

** This function first sets up the pollFds vector according to the number of servers. 
** It then uses the poll function to wait for a connection on localhost:<port>. After that, if 
** the returnValue is neither 0 nor -1, the function proceeds. In the first loop, it creates
** a communication socket for the client to interact with the server (handled in acceptNewConnection). 
** With the clientFd value, we can associate this socket with the corresponding serverFD (handled by addAssociation).

** The second loop checks if we are able to receive requests from the client, which is managed by
** the readRequest function. Here, we get the serverFD associated with this socket and use it to save it in the Request constructor.

** The third loop verifies if we can send the response to the client. Based on the information received from 
** the Request and some configurations from the Server and Route, a response message is built and sent
** using the sendResponse function.

** After communication with the clientFD is over, we close the fd and remove it from the pollFds vector.
** @return: void
*/

void Run::startServer(vector<Server>& servers)
{
    int returnValue;
    map<int, Request> requests;
    vector <struct pollfd> pollFds = loadPolls(servers);
    Client clientManager;
    
    while (true) {
        returnValue = poll(pollFds.data(), pollFds.size(), 60 * 1000);
        
        if (returnValue == 0){
            cout << "Error: poll Timeout" << endl;
            //Jogar pagine de timeout
        }
        else if (returnValue == -1){
            cout << "Error: poll failed" << endl;
        }
        else
        {
            for (size_t i = 0; i < pollFds.size(); i++) {
                if ((pollFds[i].revents & POLLIN) && (i < servers.size())) { // testar i < servers.size()
                	int clientFd = acceptNewConnection(pollFds[i].fd, pollFds);
                	clientManager.addAssociation(clientFd, servers[i]);
                }
                if (pollFds[i].revents & POLLIN) {
                	Server aaa = clientManager.getServerFd(pollFds[i].fd);
                 	Request::readRequest(pollFds, i, requests, aaa);
                }
                else if (pollFds[i].revents & POLLOUT) {
                	if (requests.find(pollFds[i].fd) != requests.end()) {
                    	clientManager.sendResponse(pollFds[i], requests);
                        pollFds.erase(pollFds.begin() + i);
                    }
                }
            }
        }
    }
}
