#include "controller.h"
#include "database.h"
#include "router.h"
#include "server.h"

#include <csignal>
#include <iostream>
#include <string>

// Global pointer so the signal handler can stop the server cleanly.
static Server* g_server = nullptr;

static void onSignal(int) {
    if (g_server) {
        std::cout << "\n[main] Shutting down...\n";
        g_server->stop();
    }
}

int main(int argc, char* argv[]) {
    int port = 8080;
    if (argc >= 2) {
        try {
            port = std::stoi(argv[1]);
        } catch (...) {
            std::cerr << "Usage: " << argv[0] << " [port]\n";
            return 1;
        }
    }

    // --- Database ---
    // Path is relative to the working directory. Run from the project root
    // so data/database.db resolves correctly.
    Database db("data/database.db");
    if (!db.load()) {
        std::cerr << "[main] Failed to load database.\n";
        return 1;
    }
    db.createTable("users");

    // --- Routes ---
    Router router;
    router.addRoute("GET",  "/",      [](const HttpRequest& r) {
        return homeController(r);
    });
    router.addRoute("GET",  "/users", [&db](const HttpRequest& r) {
        return usersController(r, db);
    });
    router.addRoute("POST", "/users", [&db](const HttpRequest& r) {
        return createUserController(r, db);
    });

    // --- Server ---
    Server server(port, router);
    g_server = &server;
    std::signal(SIGINT, onSignal);
    std::signal(SIGTERM, onSignal);

    std::cout << "[main] cpp-web-app starting on port " << port << "\n";
    if (!server.run()) {
        return 1;
    }
    return 0;
}
