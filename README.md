# C++ Web App

> **A handcrafted C++17 web platform** that turns low-level systems engineering into a product-like full-stack experience.

[Live Demo](https://cpp.subhr.in)

Built as a technical showcase, this project implements the complete web request lifecycle in modern C++: socket networking, HTTP parsing, routing, business logic, persistence, and HTML rendering — all without external web frameworks.

---

## Why this stands out

Most demos abstract away complexity. This one embraces it.

- **Framework-free architecture** with full control over the request pipeline
- **POSIX socket server** implemented from first principles
- **Custom HTTP parser** for raw protocol-level understanding
- **File-backed persistence layer** with deterministic behavior
- **Server-side rendered HTML** with zero frontend dependencies
- **Thread-per-connection model** for concurrency fundamentals

This is ideal for technical leaders evaluating engineering depth, architecture thinking, and systems-level craftsmanship.

---

## Feature surface

| Route | Method | Purpose |
|------|--------|---------|
| `/` | GET | Product-style landing page |
| `/users` | GET | User listing and create form |
| `/users` | POST | User creation (`name`, `age`) |

Core capabilities:

- HTTP request parsing and response generation
- Route dispatch with 404 fallback
- Lightweight persistent storage in `data/database.db`
- Utility layer for trim/split/decode/escape operations

---

## Technology profile

- **Language:** C++17
- **Runtime model:** Native Linux process
- **Networking:** Raw POSIX sockets
- **Dependencies:** Standard library + pthread
- **Build system:** Makefile

---

## Quick start

### Requirements

- Linux/macOS/WSL2
- `g++` with C++17 support
- `make`

### Build

```bash
make
# alternative:
# g++ -std=c++17 -Wall -O2 -pthread src/*.cpp -o server
```

### Run

```bash
./server        # default :8080
./server 9090   # custom port
```

### Try it

```bash
curl http://localhost:8080/
curl http://localhost:8080/users
curl -X POST -d "name=Alice&age=21" http://localhost:8080/users
```

---

## Repository structure

```text
cpp-web-app/
├── src/
│   ├── main.cpp         # startup + route registration
│   ├── server.cpp       # socket lifecycle + threading
│   ├── parser.cpp       # raw HTTP parsing
│   ├── router.cpp       # method/path dispatch
│   ├── controller.cpp   # route business logic
│   ├── database.cpp     # file-backed storage engine
│   ├── renderer.cpp     # HTML response generation
│   └── utils.cpp        # helper utilities
├── data/
│   └── database.db      # persistent data store
├── Makefile
└── README.md
```

---

## Data model

`data/database.db` uses a simple table-like text format:

```text
users:
1,John,20
2,Alice,21
```

Supported operations in the storage layer:

- `createTable`
- `insert` (auto-increment id)
- `select`

---

## Deployment example (Nginx reverse proxy)

```nginx
server {
    listen 80;
    server_name your.domain.or.ip;

    location / {
        proxy_pass         http://127.0.0.1:8080;
        proxy_http_version 1.1;
        proxy_set_header   Host $host;
        proxy_set_header   X-Real-IP $remote_addr;
        proxy_set_header   X-Forwarded-For $proxy_add_x_forwarded_for;
    }
}
```

---

## Engineering intent

This repository is intentionally educational and architecture-driven.

It is a strong technical experiment for:

- systems programming interviews
- backend architecture discussions
- engineering leadership demos
- teaching protocol-level web fundamentals

---

## Scope boundaries

Not intended as production infrastructure. Out of scope by design:

- authentication/authorization
- TLS termination in-process
- SQL/indexing/query optimizer
- advanced event loops (`epoll`, `io_uring`)
- horizontal scaling controls

---

## License

Educational / public domain.
