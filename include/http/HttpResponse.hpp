#pragma once

#include <string>

struct HttpResponse {
    std::string version = "HTTP/1.1";
    int status_code = 200;
    std::string status_message = "OK";
    std::string content_type = "text/plain";
    std::string body;

    std::string toString() const;
};