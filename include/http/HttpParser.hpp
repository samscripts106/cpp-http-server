#pragma once

#include "HttpRequest.hpp"
#include <string>

HttpRequest parsedRequest(const std::string& rawRequest);