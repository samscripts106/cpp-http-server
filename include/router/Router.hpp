#pragma once 

#include "http/HttpRequest.hpp"
#include "http/HttpResponse.hpp"

class Router{
    public: 
    HttpResponse route(const HttpRequest& request);
};
