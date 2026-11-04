#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include "http/HttpRequest.hpp"
#include "http/HttpParser.hpp"
#include "http/HttpResponse.hpp"

int main(){
    int server_fd = socket(AF_INET, SOCK_STREAM, 0); //socket() asks the OS to create a socket and returns an integer identifying it with our process (That integer is a file descriptor (FD)). This descriptor is later used to refer to this socket

    if(server_fd == -1){ //If socket creation fails
        perror("socket");
        return 1;
    }

    int opt = 1;
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1){ //setsockopt() configures an option on the socket. If this op fails, we close the already created socket before exiting
        perror("setsockopt");
        close(server_fd);
        return 1;
    }

    //Describing the server's IP address & port
    sockaddr_in address{};
    address.sin_family = AF_INET; //this address uses IPV4
    address.sin_port = htons(8080); //the server will use port 8080

    if(inet_pton(AF_INET, "127.0.0.1", &address.sin_addr) != 1){ //inet_ptons converts the human readable IP address "127.0.0.1" into the binary representation req by the socket API
        std::cerr << "Invalid server IP address\n";
        close(server_fd);
        return 1;
    }

    if(bind(server_fd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == -1){ //bind() associates the socket with a local IP address & port
        perror("bind");
        close(server_fd);
        return 1;
    }

    if(listen(server_fd, 5) == -1){ //listen() changes the socket into a passive listening socket that can accept incoming connection reqs (5 is the requested backlog value for pending connections)
        perror("listen");
        close(server_fd);
        return 1;
    }

    std::cout << "Server listening on 127.0.0.1:8080\n";

    int client_fd = accept(server_fd, nullptr, nullptr); //accept() waits for an incoming connection. When succesful it returns a new file descriptor representing the connection to that particular client

    if(client_fd == -1){
        perror("accept");
        close(server_fd);
        return 1;
    }

    char buffer[1024];

    ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);

    if (bytes_received <= 0) {
        std::cerr << "Failed to receive request\n";
        close(client_fd);
        close(server_fd);
        return 1;
    }

    std::string request(buffer, bytes_received);
    //HTTP req string:
    //GET /users HTTP/1.1\r\n
    //Host: 127.0.0.1:8080\r\n
    //User-Agent: curl/8.16.0\r\n
    //Accept: */*\r\n
    //\r\n

    HttpRequest parsed_request = parsedRequest(request);

    std::cout << "Method: " << parsed_request.method << '\n';
    std::cout << "Path: " << parsed_request.path << '\n';
    std::cout << "Version: " << parsed_request.version << '\n';

    for (const auto& [key, value] : parsed_request.headers) {
                std::cout << key << " = " << value << '\n';
    }

    HttpResponse response;
    response.body = "Hello, world!";

    std::string response_data = response.toString();

    send(client_fd, response_data.c_str(), response_data.size(), 0);

    close(client_fd);
    close(server_fd);

    return 0;
}