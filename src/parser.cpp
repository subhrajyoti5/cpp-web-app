#include "parser.h"
#include "utils.h"

#include <algorithm>
#include <cctype>
#include <sstream>

// Convert a string to lowercase (used for case-insensitive header names).
static std::string toLower(std::string s) {
    for (char& c : s) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return s;
}

bool parseHttpRequest(const std::string& raw, HttpRequest& out) {
    // HTTP requests separate headers from body with a blank line (CRLF CRLF).
    auto headerEnd = raw.find("\r\n\r\n");
    if (headerEnd == std::string::npos) {
        // Incomplete request — caller may need to recv() more data.
        return false;
    }

    std::string headerSection = raw.substr(0, headerEnd);
    out.body = raw.substr(headerEnd + 4);

    std::istringstream stream(headerSection);
    std::string requestLine;
    if (!std::getline(stream, requestLine)) {
        return false;
    }
    // getline leaves a trailing '\r' when lines are CRLF-terminated.
    if (!requestLine.empty() && requestLine.back() == '\r') {
        requestLine.pop_back();
    }

    // Request line: METHOD SP PATH SP HTTP/VERSION
    auto parts = split(requestLine, ' ');
    if (parts.size() < 2) {
        return false;
    }
    out.method = parts[0];
    // Strip query string for routing; we don't need it for this app.
    auto q = parts[1].find('?');
    out.path = (q == std::string::npos) ? parts[1] : parts[1].substr(0, q);

    out.headers.clear();
    std::string line;
    while (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (line.empty()) continue;

        auto colon = line.find(':');
        if (colon == std::string::npos) continue;

        std::string key = toLower(trim(line.substr(0, colon)));
        std::string value = trim(line.substr(colon + 1));
        out.headers[key] = value;
    }

    // If Content-Length is present and we don't yet have the full body,
    // signal incomplete so the server can keep reading.
    auto it = out.headers.find("content-length");
    if (it != out.headers.end()) {
        size_t expected = 0;
        try {
            expected = static_cast<size_t>(std::stoul(it->second));
        } catch (...) {
            expected = 0;
        }
        if (out.body.size() < expected) {
            return false;
        }
        // Truncate any extra bytes past Content-Length (keep it simple).
        out.body.resize(expected);
    }

    return true;
}
