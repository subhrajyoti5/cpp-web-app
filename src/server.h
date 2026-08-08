#pragma once

#include "router.h"

#include <atomic>
#include <string>

// Blocking TCP server using POSIX sockets.
// Spawns one std::thread per accepted connection (see PRD §4.7).
class Server {
public:
    Server(int port, Router& router);

    // Bind, listen, and accept loop. Blocks until stop() is called
    // or a fatal error occurs. Returns false if setup fails.
    bool run();

    void stop();

private:
    int port_;
    Router& router_;
    std::atomic<bool> running_{false};
    int listenFd_ = -1;

    // Handle a single client connection (runs on its own thread).
    void handleClient(int clientFd);
};
