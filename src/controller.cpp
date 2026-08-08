#include "controller.h"
#include "renderer.h"
#include "utils.h"

#include <iostream>
#include <sstream>

std::string httpResponse(int status, const std::string& statusText,
                         const std::string& contentType,
                         const std::string& body) {
    std::ostringstream oss;
    oss << "HTTP/1.1 " << status << " " << statusText << "\r\n"
        << "Content-Type: " << contentType << "\r\n"
        << "Content-Length: " << body.size() << "\r\n"
        << "Connection: close\r\n"
        << "\r\n"
        << body;
    return oss.str();
}

std::string httpRedirect(const std::string& location) {
    std::ostringstream oss;
    oss << "HTTP/1.1 302 Found\r\n"
        << "Location: " << location << "\r\n"
        << "Content-Length: 0\r\n"
        << "Connection: close\r\n"
        << "\r\n";
    return oss.str();
}

std::string homeController(const HttpRequest& /*req*/) {
    return httpResponse(200, "OK", "text/html; charset=utf-8", renderHomePage());
}

std::string usersController(const HttpRequest& /*req*/, Database& db) {
    auto users = db.select("users");
    return httpResponse(200, "OK", "text/html; charset=utf-8",
                        renderUsersPage(users));
}

std::string createUserController(const HttpRequest& req, Database& db) {
    auto fields = parseFormBody(req.body);

    auto nameIt = fields.find("name");
    auto ageIt  = fields.find("age");

    if (nameIt == fields.end() || ageIt == fields.end() ||
        nameIt->second.empty() || ageIt->second.empty()) {
        return httpResponse(400, "Bad Request", "text/html; charset=utf-8",
                            renderErrorPage("Name and age are required."));
    }

    // Light validation: age should be a non-negative integer.
    try {
        int age = std::stoi(ageIt->second);
        if (age < 0 || age > 150) {
            return httpResponse(400, "Bad Request", "text/html; charset=utf-8",
                                renderErrorPage("Age must be between 0 and 150."));
        }
    } catch (...) {
        return httpResponse(400, "Bad Request", "text/html; charset=utf-8",
                            renderErrorPage("Age must be a number."));
    }

    int id = db.insert("users", {nameIt->second, ageIt->second});
    if (id < 0) {
        return httpResponse(500, "Internal Server Error", "text/html; charset=utf-8",
                            renderErrorPage("Failed to save user."));
    }

    std::cout << "[controller] Inserted user id=" << id
              << " name=" << nameIt->second << "\n";

    // PRG pattern: redirect so a browser refresh doesn't re-POST.
    return httpRedirect("/users");
}
