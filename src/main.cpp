#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

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

    char buffer[1024]; //container where the received data is stored
    ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer), 0); //recv() is a system call that receives data from connected socket

    if(bytes_received > 0){
        std::cout<<"Received "<< bytes_received << "bytes\n";

        std::cout.write(buffer, bytes_received);
        std::cout<<'\n'; 

        const char* message = "Data received successfully!\n";
        send(client_fd, message, std::strlen(message), 0);
    }
    else if (bytes_received == 0){
        std::cout<<"Client disconnected\n";
    }else{
        perror("recv");
    }

    close(client_fd); //closes the accepted connection
    close(server_fd); //closes the listening socket

    return 0;
}