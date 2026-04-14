#include <atomic>
#include <chrono>
#include <csignal>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <unordered_set>

#include "ConcurrentTCPServer.hpp"
#include "connection.h"

namespace {
std::atomic<int> g_connected{0};
std::atomic<int> g_closed{0};
std::mutex g_mu;
std::unordered_set<std::string> g_threads;

std::string tid_to_string() {
    std::ostringstream oss;
    oss << std::this_thread::get_id();
    return oss.str();
}

void record_thread() {
    std::lock_guard<std::mutex> lk(g_mu);
    g_threads.insert(tid_to_string());
}
}

int main(int argc, char** argv) {
    uint16_t port = 19090;
    size_t workers = 4;
    size_t idle_timeout_sec = 2;

    if (argc > 1) port = static_cast<uint16_t>(std::stoi(argv[1]));
    if (argc > 2) workers = static_cast<size_t>(std::stoul(argv[2]));
    if (argc > 3) idle_timeout_sec = static_cast<size_t>(std::stoul(argv[3]));

    std::cout << "[server] port=" << port
              << " workers=" << workers
              << " idle_timeout_sec=" << idle_timeout_sec << std::endl;

    ConcurrentTCPServer server(port, workers);

    server.SetConnectedCallBack([](const auto& conn) {
        record_thread();
        int c = ++g_connected;
        std::cout << "[connected] id=" << conn->GetId()
                  << " thread=" << tid_to_string()
                  << " total=" << c << std::endl;
    });

    server.SetMessageCallBack([](const auto& conn, Buffer& buf) {
        record_thread();
        if (buf.Size() > 0) {
            std::string payload = buf.Read();
            conn->Send("echo:" + payload);
        }
    });

    server.SetCloseCallBack([](const auto& conn) {
        record_thread();
        int c = ++g_closed;
        std::cout << "[closed] id=" << conn->GetId()
                  << " thread=" << tid_to_string()
                  << " total=" << c << std::endl;
    });

    server.EnableInactiveRelease(idle_timeout_sec);

    server.AddTimeTask([&server]() {
        std::lock_guard<std::mutex> lk(g_mu);
        std::cout << "[stats] connected=" << g_connected.load()
                  << " closed=" << g_closed.load()
                  << " unique_callback_threads=" << g_threads.size() << std::endl;
        server.AddTimeTask([&server]() {
            std::lock_guard<std::mutex> lk2(g_mu);
            std::cout << "[stats] connected=" << g_connected.load()
                      << " closed=" << g_closed.load()
                      << " unique_callback_threads=" << g_threads.size() << std::endl;
            server.AddTimeTask([&server]() {
                std::lock_guard<std::mutex> lk3(g_mu);
                std::cout << "[stats] connected=" << g_connected.load()
                          << " closed=" << g_closed.load()
                          << " unique_callback_threads=" << g_threads.size() << std::endl;
            }, 1);
        }, 1);
    }, 1);

    server.Run();
    return 0;
}
