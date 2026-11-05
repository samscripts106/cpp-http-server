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
#include "server/Server.hpp"

int main(){
    Server server;

    server.registerRoute("GET", "/", [](const HttpRequest& request) {
        HttpResponse response;
        response.body = "Welcome to my C++ server!";
        return response;
    });

    server.registerRoute("GET", "/hello", [](const HttpRequest& request) {
        HttpResponse response;
        response.body = "Hello there!";
        return response;
    });

    server.registerRoute("POST", "/hello", [](const HttpRequest& request) {
        HttpResponse response;
        response.body = "POST Request received!";
        return response;
    });

    server.start();

    return 0;
}