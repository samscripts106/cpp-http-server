#pragma once
#include <string>
class Server{
public:
    void start(); //manages the server life cycle

private:
    int createSocket();
    void handleClient(int client_fd);
    std::string readRequest(int client_fd);
};