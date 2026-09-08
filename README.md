# cpp-web-app

<p>
  <strong>Educational single-binary web application built fully in C++.</strong><br>
  Raw sockets, custom HTTP parsing, routing, controllers, rendering, and file-backed persistence.
</p>

<p>
  <img src="https://img.shields.io/badge/C%2B%2B-17%2B-00599C?logo=c%2B%2B&logoColor=white">
  <img src="https://img.shields.io/badge/Build-Make-6D00CC?logo=gnu&logoColor=white">
  <img src="https://img.shields.io/badge/Networking-POSIX%20Sockets-0A7EA4">
  <img src="https://img.shields.io/badge/Storage-Flat%20File-4CAF50">
  <img src="https://img.shields.io/badge/Status-Learning%20Project-blue">
</p>

---

## Overview

`cpp-web-app` is a monolithic C++17 web app designed to demonstrate how web backends work without frameworks.

It includes:

- A blocking TCP server using POSIX sockets
- A custom HTTP request parser
- Method/path routing with controller handlers
- Server-rendered HTML responses
- A simple file-backed database (`data/database.db`)
- Concurrent request handling with one detached thread per connection

---

## Core Capabilities

### HTTP Server
- Socket lifecycle with `socket`, `bind`, `listen`, `accept`, `recv`, `send`
- Listens on configurable port (default `8080`)
- Graceful shutdown support for `SIGINT` and `SIGTERM`

### Routing & Controllers
- Route registration by method + path
- Built-in routes:
  - `GET /` → Home page
  - `GET /users` → Users list page
  - `POST /users` → Create user and redirect to `/users`
- 404 response for unknown routes

### Parsing & Validation
- Parses request line, headers, and body from raw HTTP bytes
- Honors `Content-Length` for complete body reads
- Form parsing for `application/x-www-form-urlencoded`
- Basic input validation for required user fields and numeric age range

### Persistence
- Custom in-memory table store with flat-file persistence
- Auto-increment IDs for inserted records
- Immediate persistence after writes

---

## Architecture

```text
Browser / Client
       │
       ▼
C++ Server (POSIX sockets)
       │
       ▼
HTTP Parser
       │
       ▼
Router
   ┌───┴───────────┐
   ▼               ▼
Controllers     404 Handler
   │
   ▼
Renderer + Database (data/database.db)
```

Project layout:

```text
src/
├── main.cpp        # bootstrap: db init, route registration, server startup
├── server.*        # TCP server and per-connection request handling
├── parser.*        # HTTP request parsing
├── router.*        # route table + request dispatch
├── controller.*    # endpoint business logic + HTTP response helpers
├── renderer.*      # HTML generation
├── database.*      # flat-file table storage
└── utils.*         # split/trim/form decoding/html escaping helpers

Makefile            # build, run, clean
server              # compiled binary output (after build)
data/database.db    # persisted application data
```

---

## Tech Stack

- **Language:** C++17
- **Build Tool:** GNU Make + g++
- **Networking:** POSIX sockets (Linux/macOS/WSL)
- **Concurrency:** `std::thread` (thread-per-connection)
- **Storage:** Custom flat-file DB (`data/database.db`)
- **Rendering:** Server-side HTML string generation

---

## Quick Start

### 1) Prerequisites
- `g++` with C++17 support
- `make`

### 2) Build
```bash
make
```

### 3) Run
```bash
./server
```

Run on a custom port:
```bash
./server 9090
```

Then open:
- `http://localhost:8080/`
- `http://localhost:8080/users`

---

## Make Targets

| Target | Purpose |
|---|---|
| `make` / `make all` | Build `server` from `src/*.cpp` |
| `make run` | Build and run `./server` |
| `make clean` | Remove object files and binary |

---

## Example Requests

Create a user:
```bash
curl -i -X POST http://localhost:8080/users \
  -H "Content-Type: application/x-www-form-urlencoded" \
  --data "name=Alice&age=21"
```

List users page:
```bash
curl -i http://localhost:8080/users
```

---

## Notes

- This project is intentionally educational and keeps logic simple.
- Data is persisted to `data/database.db`; keep that file if you want records across restarts.
- For reverse proxy deployment, place Nginx in front of this app and proxy to the server port.
