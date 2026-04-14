#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <atomic>
#include <chrono>
#include <cstring>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

struct ClientResult {
    bool connected = false;
    bool closed_by_server = false;
    bool got_echo = false;
};

int connect_once(const std::string& ip, uint16_t port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return -1;

    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) != 1) {
        close(fd);
        return -1;
    }

    if (connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        close(fd);
        return -1;
    }
    return fd;
}

int main(int argc, char** argv) {
    std::string ip = "127.0.0.1";
    uint16_t port = 19090;
    int clients = 40;
    int hold_sec = 6;

    if (argc > 1) ip = argv[1];
    if (argc > 2) port = static_cast<uint16_t>(std::stoi(argv[2]));
    if (argc > 3) clients = std::stoi(argv[3]);
    if (argc > 4) hold_sec = std::stoi(argv[4]);

    std::cout << "[client] ip=" << ip << " port=" << port
              << " clients=" << clients << " hold_sec=" << hold_sec << std::endl;

    std::vector<ClientResult> results(static_cast<size_t>(clients));
    std::vector<std::thread> threads;
    threads.reserve(static_cast<size_t>(clients));

    for (int i = 0; i < clients; ++i) {
        threads.emplace_back([&, i]() {
            int fd = connect_once(ip, port);
            if (fd < 0) return;
            results[static_cast<size_t>(i)].connected = true;

            // Send one message then stay idle; server should close idle connection by time wheel.
            const std::string msg = "hello-" + std::to_string(i) + "\\n";
            ssize_t wn = send(fd, msg.data(), msg.size(), 0);
            if (wn > 0) {
                char echo_buf[256];
                ssize_t rn = recv(fd, echo_buf, sizeof(echo_buf), 0);
                if (rn > 0) results[static_cast<size_t>(i)].got_echo = true;
            }

            auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(hold_sec);
            while (std::chrono::steady_clock::now() < deadline) {
                char buf[64];
                ssize_t rn = recv(fd, buf, sizeof(buf), MSG_DONTWAIT);
                if (rn == 0) {
                    results[static_cast<size_t>(i)].closed_by_server = true;
                    break;
                }
                if (rn < 0) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(50));
                        continue;
                    }
                    break;
                }
            }

            close(fd);
        });
    }

    for (auto& t : threads) t.join();

    int connected = 0;
    int closed_by_server = 0;
    int got_echo = 0;
    for (const auto& r : results) {
        connected += r.connected ? 1 : 0;
        closed_by_server += r.closed_by_server ? 1 : 0;
        got_echo += r.got_echo ? 1 : 0;
    }

    std::cout << "[result] connected=" << connected
              << " echo_ok=" << got_echo
              << " idle_closed_by_server=" << closed_by_server << std::endl;

    if (connected == clients && closed_by_server >= clients * 9 / 10) {
        std::cout << "[result] PASS" << std::endl;
        return 0;
    }

    std::cout << "[result] FAIL" << std::endl;
    return 1;
}
