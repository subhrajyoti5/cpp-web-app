#include "server.h"
#include "parser.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <thread>
#include <vector>

Server::Server(int port, Router& router) : port_(port), router_(router) {}

bool Server::run() {
    // 1. Create a TCP socket (IPv4, stream).
    listenFd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (listenFd_ < 0) {
        std::cerr << "[server] socket() failed: " << std::strerror(errno) << "\n";
        return false;
    }

    // Allow quick rebinding after restart (avoids "Address already in use").
    int opt = 1;
    if (setsockopt(listenFd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "[server] setsockopt(SO_REUSEADDR) failed: "
                  << std::strerror(errno) << "\n";
        close(listenFd_);
        return false;
    }

    // 2. Bind to 0.0.0.0:port so Nginx (or curl) can reach us.
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(static_cast<uint16_t>(port_));

    if (bind(listenFd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::cerr << "[server] bind() failed: " << std::strerror(errno) << "\n";
        close(listenFd_);
        return false;
    }

    // 3. Start listening. Backlog of 64 pending connections is plenty for demos.
    if (listen(listenFd_, 64) < 0) {
        std::cerr << "[server] listen() failed: " << std::strerror(errno) << "\n";
        close(listenFd_);
        return false;
    }

    running_ = true;
    std::cout << "[server] Listening on port " << port_ << "\n";

    // 4. Accept loop — each connection gets its own thread.
    while (running_) {
        sockaddr_in clientAddr{};
        socklen_t clientLen = sizeof(clientAddr);
        int clientFd = accept(listenFd_, reinterpret_cast<sockaddr*>(&clientAddr),
                              &clientLen);
        if (clientFd < 0) {
            if (!running_) break;
            // EINTR is normal if a signal interrupts accept; just retry.
            if (errno == EINTR) continue;
            std::cerr << "[server] accept() failed: " << std::strerror(errno) << "\n";
            continue;
        }

        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, ip, sizeof(ip));
        std::cout << "[server] Connection from " << ip << ":"
                  << ntohs(clientAddr.sin_port) << "\n";

        // Detach so the accept loop never waits on long-lived handlers.
        // For a learning server this is fine; a production one would track
        // threads or use a pool and join on shutdown.
        std::thread(&Server::handleClient, this, clientFd).detach();
    }

    if (listenFd_ >= 0) {
        close(listenFd_);
        listenFd_ = -1;
    }
    return true;
}

void Server::stop() {
    running_ = false;
    if (listenFd_ >= 0) {
        // Unblock accept() by closing the listening socket.
        close(listenFd_);
        listenFd_ = -1;
    }
}

void Server::handleClient(int clientFd) {
    // Read the full request. We loop until the parser says we have a complete
    // message (headers + Content-Length body), or we hit a size cap.
    constexpr size_t kMaxRequest = 1 << 20; // 1 MiB — plenty for forms
    std::string buffer;
    buffer.reserve(4096);
    char chunk[4096];

    HttpRequest req;
    bool parsed = false;

    while (buffer.size() < kMaxRequest) {
        ssize_t n = recv(clientFd, chunk, sizeof(chunk), 0);
        if (n < 0) {
            std::cerr << "[server] recv() failed: " << std::strerror(errno) << "\n";
            close(clientFd);
            return;
        }
        if (n == 0) {
            // Client closed before sending a full request.
            break;
        }
        buffer.append(chunk, static_cast<size_t>(n));

        if (parseHttpRequest(buffer, req)) {
            parsed = true;
            break;
        }
    }

    std::string response;
    if (!parsed) {
        response =
            "HTTP/1.1 400 Bad Request\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 11\r\n"
            "Connection: close\r\n"
            "\r\n"
            "Bad Request";
    } else {
        std::cout << "[server] " << req.method << " " << req.path << "\n";
        response = router_.route(req);
    }

    // send() may write fewer bytes than requested; loop until done.
    size_t totalSent = 0;
    while (totalSent < response.size()) {
        ssize_t n = send(clientFd, response.data() + totalSent,
                         response.size() - totalSent, 0);
        if (n < 0) {
            std::cerr << "[server] send() failed: " << std::strerror(errno) << "\n";
            break;
        }
        totalSent += static_cast<size_t>(n);
    }

    close(clientFd);
}
