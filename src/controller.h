#pragma once

#include "database.h"
#include "parser.h"

#include <string>

// Build a full HTTP response (status + headers + body).
std::string httpResponse(int status, const std::string& statusText,
                         const std::string& contentType,
                         const std::string& body);

// 302 redirect helper (used after successful POST /users).
std::string httpRedirect(const std::string& location);

// Controllers — pure business logic + HTML via the renderer.
std::string homeController(const HttpRequest& req);
std::string usersController(const HttpRequest& req, Database& db);
std::string createUserController(const HttpRequest& req, Database& db);
