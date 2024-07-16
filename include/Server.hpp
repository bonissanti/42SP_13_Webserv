#ifndef SERVER_HPP
#define SERVER_HPP

#include <map>
#include <string>
#include <vector>

#include "Client.hpp"
#include "Config.hpp"
#include "Request.hpp"
#include "Response.hpp"

class Server {
    public:
        Server(const std::string& config_path);
        void start();

    private:
        void loadConfig(const std::string& config_path);
        void handleClient(int client_socket);

        Config              config_;
        int                 server_socket_;
        std::vector<Client> clients_;
};

#endif  // SERVER_HPP
