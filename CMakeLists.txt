cmake_minimum_required(VERSION 3.20)

project(cpp_http_server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(http_server
    src/main.cpp
    src/http/HttpParser.cpp
    src/http/HttpResponse.cpp
    src/server/Server.cpp
)

target_include_directories(http_server PRIVATE include)