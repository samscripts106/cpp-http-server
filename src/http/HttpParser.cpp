#include "http/HttpParser.hpp"

#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>

HttpRequest parsedRequest(const std::string& rawRequest){
    HttpRequest parsed_request;

    size_t line_end = rawRequest.find("\r\n"); //searches the first occurences of \r\n

    size_t headers_end = rawRequest.find("\r\n\r\n"); //\r\n\r\n identifies the boundary between headers and the optional request body

    if(line_end != std::string::npos){ //
        std::string request_line = rawRequest.substr(0,line_end); //extracts characters starting at index 0, for a length of line_end //Result: GET /users HTTP/1.1

        std::istringstream iss(request_line); //Allows us to read a string as though it were an input stream

        if (!(iss >> parsed_request.method >> parsed_request.path >> parsed_request.version)) {
            std::cerr << "Invalid HTTP request line\n";
            return parsed_request;
        } //>> operator extracts whitespace-separated tokens

        //Check for invalid extra tokens in the request line
        std::string extra;
        if(iss>>extra){
            std::cerr<<"Invalid HTTP request line: extra token\n";
            return parsed_request;
        }

        //Validate the HTTP version
        if(parsed_request.version != "HTTP/1.1"){
            std::cerr<<"Unsupported HTTP version\n";
            return parsed_request;
        }

        //Validate the HTTP method (for time being GEt & POST are only allowed)
        if (parsed_request.method != "GET" &&
            parsed_request.method != "POST") {
            std::cerr << "Unsupported HTTP method\n";
            parsed_request.unsupported_method = true;
            return parsed_request;
        }

        //Validates requested route (rejects hello but accept /hello)
        if (parsed_request.path.empty() || parsed_request.path[0] != '/') {
            std::cerr << "Invalid request target\n";
            return parsed_request;
        }

        if(headers_end != std::string::npos){
            std::string headers = rawRequest.substr(line_end+2, headers_end - (line_end+2));

            std::istringstream header_stream(headers); //lets us read headers line by line
            std::string header_line; //temporarily holds each line

            while (std::getline(header_stream, header_line)){ //Iterate through each headers
                size_t colon_pos = header_line.find(':');

                if(colon_pos != std::string::npos){
                    std::string key = header_line.substr(0, colon_pos);
                    std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c){
                        return std::tolower(c);
                    }); //converts every character of key to lowercase thus eradicating redundancy
                    std::string value = header_line.substr(colon_pos+1);

                    //Whitespace trimming
                    size_t start = value.find_first_not_of(" \t");
                    size_t end = value.find_last_not_of(" \t");

                    value = (start == std::string::npos)? "" : value.substr(start, end-start+1);

                    parsed_request.headers[key] = value;
                }
            }
        } 
        parsed_request.valid = true; 
    }
    return parsed_request;
}