#include "Client.hpp"

Client::Client(){
	
}

Client::~Client(){
	
}

Client::ClientException::ClientException(const string& message) : msg(message){}
Client::ClientException::~ClientException() throw(){}

const char* Client::ClientException::what() const throw(){
	return (msg.c_str());
}


void Client::addAssociation(int clientFd, Server server){
	_fdsMap[clientFd] = server;
}

Server	Client::getServerFd(int clientFd){
	Server null;
	if (_fdsMap.find(clientFd) != _fdsMap.end())
		return (_fdsMap[clientFd]);
	else{
		cerr << RED << "Error: serverFd not found for request" << RESET;
	}
	return (null);
}
