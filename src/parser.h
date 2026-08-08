#pragma once

#include <string>
#include <map>

// Parsed representation of a single HTTP request.
struct HttpRequest {
    std::string method;                          // "GET" or "POST"
    std::string path;                            // e.g. "/users"
    std::map<std::string, std::string> headers;  // lowercased keys
    std::string body;                            // raw body (POST forms, etc.)
};

// Turn a raw byte buffer received from the socket into an HttpRequest.
// Returns true on success, false if the buffer is incomplete or malformed.
bool parseHttpRequest(const std::string& raw, HttpRequest& out);
