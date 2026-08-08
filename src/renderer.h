#pragma once

#include "database.h"

#include <string>
#include <vector>

// Server-side HTML generation — plain C++ strings, no templating engine.

std::string renderHomePage();
std::string renderUsersPage(const std::vector<Record>& users);
std::string renderNotFoundPage(const std::string& path);
std::string renderErrorPage(const std::string& message);

// Wrap a body fragment in a common HTML shell (doctype, head, nav, css link).
std::string wrapLayout(const std::string& title, const std::string& bodyHtml);
