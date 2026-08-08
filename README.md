# cpp-web-app

A monolithic full-stack web application written entirely in **C++17** for educational purposes.

Every layer of the stack — HTTP server, router, business logic, file-backed database, and HTML rendering — is implemented with raw POSIX sockets and the standard library. No external web frameworks, no SQL engine, no JavaScript frontend.

## Features

| Route | Method | Description |
|-------|--------|-------------|
| `/` | GET | Landing page |
| `/users` | GET | List all users + add form |
| `/users` | POST | Create a user (`name`, `age` form fields) |

- Custom HTTP request parser
- Route table with 404 fallback
- Flat-file persistence (`data/database.db`)
- Server-rendered raw HTML (no CSS / JS)
- One thread per connection

## Requirements

- **OS:** Linux (Ubuntu VPS/EC2 recommended), macOS, or WSL2 on Windows
- **Compiler:** `g++` with C++17 support
- **Libraries:** POSIX sockets only (stock on Linux)

## Build

```bash
make
# or:
# g++ -std=c++17 -Wall -O2 -pthread src/*.cpp -o server
```

## Run

From the **project root** (so `data/` and `public/` resolve correctly):

```bash
./server          # listens on :8080
./server 9090     # custom port
```

Then open `http://localhost:8080` in a browser, or:

```bash
curl http://localhost:8080/
curl http://localhost:8080/users
curl -X POST -d "name=Alice&age=21" http://localhost:8080/users
```

## Project layout

```
cpp-web-app/
├── src/
│   ├── main.cpp         # entry point, route registration
│   ├── server.cpp       # socket bind/listen/accept + thread-per-connection
│   ├── parser.cpp       # raw HTTP → method/path/headers/body
│   ├── router.cpp       # (method, path) → controller
│   ├── controller.cpp   # business logic for each route
│   ├── database.cpp     # file-backed table store
│   ├── renderer.cpp     # HTML string generation
│   └── utils.cpp        # split, trim, url-decode, html-escape
├── data/
│   └── database.db      # persistent flat-file storage
├── Makefile
└── README.md
```

## Database format

`data/database.db` is a simple multi-table text file:

```
users:
1,John,20
2,Alice,21
```

Supported operations (in-process API, not a query language):

- **createTable** — ensure a named table exists
- **insert** — append a row (auto-increment id), then save to disk
- **select** — return all rows for a table

## Nginx reverse proxy

On a VPS, put Nginx on port 80 and proxy to the C++ server on 8080:

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

Typical deploy steps on Ubuntu:

```bash
sudo apt update && sudo apt install -y g++ make nginx
# copy project, then:
make
./server &                          # or use systemd
sudo cp nginx-snippet.conf ...      # or edit /etc/nginx/sites-available/default
sudo systemctl reload nginx
```

## Learning path

This codebase follows the PRD build order:

1. Socket server (bind / listen / accept / recv / send)
2. HTTP parser
3. Router + controllers
4. File-backed database
5. HTML renderer
6. Thread-per-connection concurrency

Read the comments in `src/` — they explain *why* each networking and persistence step exists.

## Non-goals

This is **not** production software. Intentionally omitted: auth, TLS in-process, SQL, indexing, epoll/io_uring, input hardening, horizontal scale.

## License

Educational / public domain — use freely for learning.
