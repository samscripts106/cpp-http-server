#include "http/HttpResponse.hpp"

std::string HttpResponse::toString() const {
    std::string response =
        version + " " +
        std::to_string(status_code) + " " +
        status_message + "\r\n" +
        "Content-Type: " + content_type + "\r\n" +
        "Content-Length: " + std::to_string(body.size()) + "\r\n" +
        "Connection: close\r\n" +
        "\r\n" +
        body;

    return response;
}