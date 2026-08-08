#pragma once

#include <string>
#include <vector>
#include <map>

// Split a string by a delimiter character.
std::vector<std::string> split(const std::string& s, char delim);

// Trim leading/trailing whitespace from a string.
std::string trim(const std::string& s);

// Decode application/x-www-form-urlencoded values (+ → space, %XX → byte).
std::string urlDecode(const std::string& s);

// Parse "key=value&key2=value2" form body into a map.
std::map<std::string, std::string> parseFormBody(const std::string& body);

// Escape characters that would break HTML text content.
std::string htmlEscape(const std::string& s);
