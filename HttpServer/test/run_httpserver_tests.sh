#!/bin/sh
set -e
./httpserver_test_server >/tmp/httpserver_test.log 2>&1 &
server_pid=$!
trap 'kill $server_pid 2>/dev/null || true' EXIT
sleep 1
for test_bin in \
    ./httpserver_bad_request_test \
    ./httpserver_business_timeout_test \
    ./httpserver_large_file_test \
    ./httpserver_long_connection_test \
    ./httpserver_parallel_requests_test \
    ./httpserver_pressure_test \
    ./httpserver_request_timeout_test \
    ./httpserver_timeout_connection_test

do
    echo "[RUN] $test_bin"
    "$test_bin"
done
kill $server_pid 2>/dev/null || true
