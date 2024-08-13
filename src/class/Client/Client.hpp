#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <exception>
#include "../../../include/defines.hpp"
#include "../Server/Server.hpp"

class Client{
	private:
		map<int, Server> _fdsMap;	
	
	public:
		Client();
		~Client();
		void	addAssociation(int clientFd, Server server);	
		Server	getServerFd(int clientFd);
		
		class ClientException : public std::exception{
			private:
			string msg;
			
			public:
			ClientException(const string& message);
			virtual ~ClientException() throw();
			virtual const char* what() const throw();
		};
};

#endif