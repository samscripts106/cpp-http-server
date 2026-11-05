#include "router/Router.hpp"

Router::Router() { //registers the routes
    registerRoute("GET", "/", [](const HttpRequest& request) {
        HttpResponse response;
        response.body = "Welcome to my C++ server!";
        return response;
    });

    registerRoute("GET", "/hello", [](const HttpRequest& request) {
        HttpResponse response;
        response.body = "Hello there!";
        return response;
    });

    registerRoute("POST", "/hello", [](const HttpRequest& request) {
        HttpResponse response;
        response.body = "POST Request received!";
        return response;
    });
}

HttpResponse Router::route(const HttpRequest& request) { //searches for a matching route and executes its handler
    std::string key = request.method+" "+request.path; //Construct the key eg: "GET /hello"

    auto it = routes.find(key); //find the route in the unordered map

    if(it != routes.end()) return it->second(request); //executes that handler with the request (it.second is the handler function)

    HttpResponse response;

    response.status_code = 404;
    response.status_message = "Not Found";
    response.body = "404 - Page Not Found";

    return response;
}

void Router::registerRoute(const std::string& method, const std::string& path, Handler handler){
    std::string key = method + " " + path;
    routes[key] = handler;
}