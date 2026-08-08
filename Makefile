# cpp-web-app — single-binary educational web server
# Requires: g++ with C++17, POSIX sockets (Linux / macOS / WSL)

CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -O2 -pthread
LDFLAGS  := -pthread

SRC_DIR  := src
SRCS     := $(wildcard $(SRC_DIR)/*.cpp)
OBJS     := $(SRCS:.cpp=.o)
TARGET   := server

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
