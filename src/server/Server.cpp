#include "server/Server.hpp"

#include "http/HttpRequest.hpp"
#include "http/HttpParser.hpp"
#include "http/HttpResponse.hpp"

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

//CREATES AND PREPARES THE LISTENING SOCKET
int Server::createSocket() { 
    int server_fd = socket(AF_INET, SOCK_STREAM, 0); //socket() asks the OS to create a socket and returns an integer identifying it with our process (That integer is a file descriptor (FD)). This descriptor is later used to refer to this socket

    if(server_fd == -1){ //If socket creation fails
        perror("socket");
        return -1;
    }

    int opt = 1;
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1){ //setsockopt() configures an option on the socket. If this op fails, we close the already created socket before exiting
        perror("setsockopt");
        close(server_fd);
        return -1;
    }

    //Describing the server's IP address & port
    sockaddr_in address{};
    address.sin_family = AF_INET; //this address uses IPV4
    address.sin_port = htons(8080); //the server will use port 8080

    if(inet_pton(AF_INET, "127.0.0.1", &address.sin_addr) != 1){ //inet_ptons converts the human readable IP address "127.0.0.1" into the binary representation req by the socket API
        std::cerr << "Invalid server IP address\n";
        close(server_fd);
        return -1;
    }

    if(bind(server_fd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == -1){ //bind() associates the socket with a local IP address & port
        perror("bind");
        close(server_fd);
        return -1;
    }

    if(listen(server_fd, 5) == -1){ //listen() changes the socket into a passive listening socket that can accept incoming connection reqs (5 is the requested backlog value for pending connections)
        perror("listen");
        close(server_fd);
        return -1;
    }

    return server_fd;
}

//COMMUNICATES WITH THE PARTICULAR CLIENT
void Server::handleClient(int client_fd) {
    int option = 1;

    //SO_NOSIGPIPE prevents unexpected killing of the server when any client disconnects
    if (setsockopt(client_fd, SOL_SOCKET, SO_NOSIGPIPE, &option, sizeof(option)) == -1) {
        perror("setsockopt SO_NOSIGPIPE");
        return;
    }

    std::string request = readRequest(client_fd);

    if (request.empty()) {
        std::cerr << "Failed to receive request\n";
        return;
    }

    HttpRequest parsed_request = parsedRequest(request);

    std::cout << "Method: " << parsed_request.method << '\n';
    std::cout << "Path: " << parsed_request.path << '\n';
    std::cout << "Version: " << parsed_request.version << '\n';

    for (const auto& [key, value] : parsed_request.headers) {
                std::cout << key << " = " << value << '\n';
    }

    size_t header_end = request.find("\r\n\r\n");
    size_t body_start = header_end + 4;

    std::string body = request.substr(body_start);

    std::cout<<"Request Body: "<<body<<'\n';

    HttpResponse response = router.route(parsed_request);

    std::string response_data = response.toString();

    if(!sendAll(client_fd, response_data)){
        std::cerr<<"Failed to send response\n";
    }
}

//SEQUENTIAL HANDLING OF MULTIPLE CLIENTS (Aceept Client1 -> Handle Client1 -> Close Client1 -> Accept Client2 -> Handle Client2 -> Close Client2 -> Repeat)
void Server::start(){
    int server_fd = createSocket();

    if(server_fd == -1) return;

    std::cout<<"Server listening on 127.0.0.1:8080\n";

    while(true){
        int client_fd = accept(server_fd, nullptr, nullptr);

        if(client_fd == -1){
            perror("accept");
            continue;
        }

        handleClient(client_fd);
        close(client_fd);
    }
    close(server_fd);
}

std::string Server::readRequest(int client_fd){
    std::string request;
    char buffer[1024];

    //Phase-1: Receive HTTP headers
    size_t header_end = std::string::npos;

    while(header_end == std::string::npos){
        ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);

        if(bytes_received<=0) return "";

        request.append(buffer, bytes_received);

        header_end = request.find("\r\n\r\n");
    }

    //Phase-2: Determine expected request size
    HttpRequest parsed_request = parsedRequest(request);

    size_t content_length = 0;

    if(parsed_request.headers.count("content-length")){
        content_length = std::stoul(parsed_request.headers["content-length"]);
    }

    size_t body_start = header_end + 4;

    size_t expected_size = body_start + content_length;

    //Phase-3: Receive remaining body
    while (request.size() < expected_size){
        size_t bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);

        if(bytes_received<=0) return "";

        request.append(buffer, bytes_received);
    }
    return request;
}

void Server::registerRoute(const std::string& method, const std::string& path, Router::Handler handler){ //Receives method, path and handler
    router.registerRoute(method, path, handler); //Constructs the "GET/Hello" & the router stores the handler in the unordred_map
}

bool Server::sendAll(int client_fd, const std::string& data){
    size_t total_sent = 0;

    while(total_sent < data.size()){
        ssize_t bytes_sent = send(client_fd, data.c_str() + total_sent, data.size() - total_sent, 0);

        if(bytes_sent <= 0) return false;

        total_sent += bytes_sent;
    }
    return true;
}

