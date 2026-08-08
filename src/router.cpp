#include "router.h"
#include "renderer.h"

#include <sstream>

std::string Router::key(const std::string& method, const std::string& path) {
    return method + " " + path;
}

void Router::addRoute(const std::string& method, const std::string& path, Handler handler) {
    routes_[key(method, path)] = std::move(handler);
}

std::string Router::route(const HttpRequest& req) const {
    auto it = routes_.find(key(req.method, req.path));
    if (it != routes_.end()) {
        return it->second(req);
    }

    // 404 for anything we don't know about.
    std::string html = renderNotFoundPage(req.path);
    std::ostringstream oss;
    oss << "HTTP/1.1 404 Not Found\r\n"
        << "Content-Type: text/html; charset=utf-8\r\n"
        << "Content-Length: " << html.size() << "\r\n"
        << "Connection: close\r\n"
        << "\r\n"
        << html;
    return oss.str();
}
