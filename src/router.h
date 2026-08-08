#pragma once

#include "parser.h"

#include <functional>
#include <map>
#include <string>
#include <utility>

// Controller signature: takes the parsed request, returns an HTTP response body
// as a fully-formed HTTP message (status line + headers + body).
using Handler = std::function<std::string(const HttpRequest&)>;

class Router {
public:
    // Register a handler for (method, path), e.g. ("GET", "/users").
    void addRoute(const std::string& method, const std::string& path, Handler handler);

    // Dispatch a request. Returns the full HTTP response string.
    // Unmatched routes get a simple 404 HTML page.
    std::string route(const HttpRequest& req) const;

private:
    // Key: "METHOD path"  e.g. "GET /users"
    std::map<std::string, Handler> routes_;

    static std::string key(const std::string& method, const std::string& path);
};
