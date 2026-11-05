#pragma once
#include <string>
#include "router/Router.hpp"

class Server{
public:
    void start(); //manages the server life cycle
    void registerRoute(const std::string& method, const std::string& path, Router::Handler handler);

private:
    int createSocket();
    void handleClient(int client_fd);
    std::string readRequest(int client_fd);
    Router router;
    bool sendAll (int client_fd, const std::string& data);
};