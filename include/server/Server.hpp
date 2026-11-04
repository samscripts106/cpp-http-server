#pragma once

class Server{
public:
    void start(); //manages the server life cycle

private:
    int createSocket();
    void handleClient(int client_fd);
};