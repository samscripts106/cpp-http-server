#include "router/Router.hpp"

HttpResponse Router::route(const HttpRequest& request) {
    HttpResponse response;

    if (request.path == "/") {
        response.body = "Welcome to my C++ server!";
    }
    else if (request.path == "/hello") {
        response.body = "Hello there!";
    }
    else {
        response.status_code = 404;
        response.status_message = "Not Found";
        response.body = "404 - Page Not Found";
    }

    return response;
}