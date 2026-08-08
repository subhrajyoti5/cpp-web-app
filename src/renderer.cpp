#include "renderer.h"
#include "utils.h"

std::string wrapLayout(const std::string& title, const std::string& bodyHtml) {
    return
        "<html>\n"
        "<head><title>" + htmlEscape(title) + "</title></head>\n"
        "<body>\n"
        "<p><a href=\"/\">Home</a> | <a href=\"/users\">Users</a></p>\n"
        + bodyHtml +
        "</body>\n"
        "</html>\n";
}

std::string renderHomePage() {
    std::string body =
        "<h1>cpp-web-app</h1>\n"
        "<p>A monolithic full-stack web app written entirely in C++17.</p>\n"
        "<p><a href=\"/users\">Manage Users</a></p>\n";
    return wrapLayout("Home", body);
}

std::string renderUsersPage(const std::vector<Record>& users) {
    std::string body;
    body += "<h1>Users</h1>\n";
    body += "<table border=\"1\">\n"
            "<tr><th>ID</th><th>Name</th><th>Age</th></tr>\n";

    if (users.empty()) {
        body += "<tr><td colspan=\"3\">No users yet.</td></tr>\n";
    } else {
        for (const auto& rec : users) {
            std::string id   = rec.size() > 0 ? htmlEscape(rec[0]) : "";
            std::string name = rec.size() > 1 ? htmlEscape(rec[1]) : "";
            std::string age  = rec.size() > 2 ? htmlEscape(rec[2]) : "";
            body += "<tr><td>" + id + "</td><td>" + name +
                    "</td><td>" + age + "</td></tr>\n";
        }
    }
    body += "</table>\n";

    body +=
        "<h2>Add User</h2>\n"
        "<form method=\"POST\" action=\"/users\">\n"
        "Name: <input type=\"text\" name=\"name\" required>\n"
        "Age: <input type=\"number\" name=\"age\" required>\n"
        "<button type=\"submit\">Add User</button>\n"
        "</form>\n";

    return wrapLayout("Users", body);
}

std::string renderNotFoundPage(const std::string& path) {
    std::string body =
        "<h1>404 Not Found</h1>\n"
        "<p>No route for " + htmlEscape(path) + "</p>\n"
        "<p><a href=\"/\">Home</a></p>\n";
    return wrapLayout("404", body);
}

std::string renderErrorPage(const std::string& message) {
    std::string body =
        "<h1>Error</h1>\n"
        "<p>" + htmlEscape(message) + "</p>\n"
        "<p><a href=\"/users\">Back to users</a></p>\n";
    return wrapLayout("Error", body);
}
