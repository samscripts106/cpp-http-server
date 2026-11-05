#pragma once 
#include <unordered_map>
#include <functional>
#include <string>

#include "http/HttpRequest.hpp"
#include "http/HttpResponse.hpp"

class Router{
    public: 
    using Handler = std::function<HttpResponse(const HttpRequest&)>; //any callable that accepts a const HttpRequest & returns an HttpResponse

    Router();
    
    HttpResponse route(const HttpRequest& request); 

    void registerRoute(const std:: string& method, const std::string& path, Handler handler);
    
    private:
    std::unordered_map<std::string,Handler> routes; //stores a string key mapped to a handler
};
