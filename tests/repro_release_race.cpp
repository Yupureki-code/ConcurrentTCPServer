#include <CTS/CTS.h>

#include <atomic>
#include <condition_variable>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>

int main()
{
    CTS server(9090, 0);
    std::mutex mutex;
    std::condition_variable condition;
    std::unordered_map<uint64_t, std::shared_ptr<Connection>> connections;
    std::atomic<bool> ready{false};
    std::atomic<bool> released{false};

    server.SetConnectedCallBack([&](const std::shared_ptr<Connection>& conn) {
        std::lock_guard<std::mutex> lock(mutex);
        connections.emplace(conn->GetId(), conn);
        if (connections.size() >= 2)
            ready.store(true, std::memory_order_release);
        condition.notify_all();
    });

    server.SetCloseCallBack([&](const std::shared_ptr<Connection>& conn) {
        std::lock_guard<std::mutex> lock(mutex);
        connections.erase(conn->GetId());
    });

    server.SetMessageCallBack([&](const std::shared_ptr<Connection>& conn, Buffer&) {
        if (!released.exchange(true)) {
            std::shared_ptr<Connection> victim;
            {
                std::lock_guard<std::mutex> lock(mutex);
                for (const auto& entry : connections) {
                    if (entry.first != conn->GetId()) {
                        victim = entry.second;
                        break;
                    }
                }
            }
            if (victim)
                victim->Release();
        }
        conn->Send("ok\n");
    });

    std::thread server_thread([&]() { server.Run(); });

    std::unique_lock<std::mutex> lock(mutex);
    condition.wait(lock, [&]() { return ready.load(std::memory_order_acquire); });
    lock.unlock();

    std::cout << "server_ready" << std::endl;
    server_thread.join();
    return 0;
}
