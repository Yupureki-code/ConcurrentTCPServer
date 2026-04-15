CTS_LIB_DIR:=../../CTS/output/lib
LOGGER_LIB_DIR:=../../Logger/output/lib
CTS_INCLUDE_DIR:=../../CTS/output/include
LOGGER_INCLUDE_DIR:=../../Logger/output/include

CPP_TEST_SRCS:=$(wildcard tests/*.cpp)
CPP_TEST_BINS:=$(patsubst tests/%.cpp,tests/%,$(CPP_TEST_SRCS))
SERVER_BIN:=server

.PHONY: all test cpp-tests run-cpp clean

all: $(SERVER_BIN) cpp-tests

$(SERVER_BIN): test.cpp
	@g++ -std=c++17 test.cpp \
		-I../src \
		-I$(CTS_INCLUDE_DIR) -I$(LOGGER_INCLUDE_DIR) \
		-L$(CTS_LIB_DIR) -L$(LOGGER_LIB_DIR) \
		-Wl,-rpath,$(CTS_LIB_DIR) -Wl,-rpath,$(LOGGER_LIB_DIR) \
		-lCTS -lLogger -pthread -o $(SERVER_BIN)

test: $(SERVER_BIN)

tests/%: tests/%.cpp tests/http_test_common.hpp
	@g++ -std=c++17 $< \
		-Itests \
		-I../src \
		-I$(CTS_INCLUDE_DIR) -I$(LOGGER_INCLUDE_DIR) \
		-L$(CTS_LIB_DIR) -L$(LOGGER_LIB_DIR) \
		-Wl,-rpath,$(CTS_LIB_DIR) -Wl,-rpath,$(LOGGER_LIB_DIR) \
		-lCTS -lLogger -pthread -o $@

cpp-tests: $(CPP_TEST_BINS)

run-cpp: server cpp-tests
	@set -e; \
	./$(SERVER_BIN) > /tmp/httpserver_test.log 2>&1 & \
	server_pid=$$!; \
	trap 'kill $$server_pid 2>/dev/null || true' EXIT; \
	sleep 1; \
	for bin in $(CPP_TEST_BINS); do \
		echo "[RUN] $$bin"; \
		./$$bin; \
	done; \
	kill $$server_pid 2>/dev/null || true

clean:
	@rm -f $(SERVER_BIN) $(CPP_TEST_BINS)