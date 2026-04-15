#pragma once

#include <arpa/inet.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cerrno>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <netinet/in.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <unordered_map>
#include <vector>

namespace http_test
{
inline constexpr const char* kHost = "127.0.0.1";
inline constexpr uint16_t kPort = 8080;

struct Response
{
    int status = 0;
    std::unordered_map<std::string, std::string> headers;
    std::string body;
};

inline void fail(const std::string& message)
{
    throw std::runtime_error(message);
}

inline void require(bool condition, const std::string& message)
{
    if(!condition)
        fail(message);
}

inline std::string trim(std::string value)
{
    while(!value.empty() && (value.back() == '\r' || value.back() == '\n' || value.back() == ' ' || value.back() == '\t'))
        value.pop_back();
    size_t pos = 0;
    while(pos < value.size() && (value[pos] == ' ' || value[pos] == '\t'))
        ++pos;
    return value.substr(pos);
}

class Client
{
public:
    Client()
    {
        _sock = ::socket(AF_INET, SOCK_STREAM, 0);
        if(_sock < 0)
            fail(std::string("socket failed: ") + std::strerror(errno));

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(kPort);
        if(::inet_pton(AF_INET, kHost, &addr.sin_addr) != 1)
            fail("inet_pton failed");

        if(::connect(_sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
            fail(std::string("connect failed: ") + std::strerror(errno));
    }

    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;

    ~Client()
    {
        if(_sock >= 0)
            ::close(_sock);
    }

    void set_timeout(int seconds)
    {
        timeval tv{};
        tv.tv_sec = seconds;
        tv.tv_usec = 0;
        if(::setsockopt(_sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
            fail(std::string("setsockopt failed: ") + std::strerror(errno));
    }

    void send_all(const std::string& request)
    {
        size_t offset = 0;
        while(offset < request.size())
        {
            ssize_t n = ::send(_sock, request.data() + offset, request.size() - offset, 0);
            if(n < 0)
            {
                if(errno == EINTR)
                    continue;
                fail(std::string("send failed: ") + std::strerror(errno));
            }
            offset += static_cast<size_t>(n);
        }
    }

    Response read_response()
    {
        Response response;
        std::string raw;
        char buffer[4096];
        while(raw.find("\r\n\r\n") == std::string::npos)
        {
            ssize_t n = ::recv(_sock, buffer, sizeof(buffer), 0);
            if(n < 0)
            {
                if(errno == EINTR)
                    continue;
                fail(std::string("recv header failed: ") + std::strerror(errno));
            }
            if(n == 0)
                break;
            raw.append(buffer, buffer + n);
        }

        auto header_end = raw.find("\r\n\r\n");
        require(header_end != std::string::npos, "response missing header terminator");
        std::string header_block = raw.substr(0, header_end);
        response.body = raw.substr(header_end + 4);

        std::istringstream header_stream(header_block);
        std::string status_line;
        std::getline(header_stream, status_line);
        status_line = trim(status_line);
        size_t first_space = status_line.find(' ');
        require(first_space != std::string::npos, "bad status line");
        size_t second_space = status_line.find(' ', first_space + 1);
        std::string status_str = second_space == std::string::npos
            ? status_line.substr(first_space + 1)
            : status_line.substr(first_space + 1, second_space - first_space - 1);
        response.status = std::stoi(status_str);

        std::string line;
        while(std::getline(header_stream, line))
        {
            line = trim(line);
            if(line.empty())
                continue;
            auto colon = line.find(':');
            require(colon != std::string::npos, "bad header line");
            std::string key = trim(line.substr(0, colon));
            std::string value = trim(line.substr(colon + 1));
            response.headers[key] = value;
        }

        size_t content_length = 0;
        auto it = response.headers.find("Content-Length");
        if(it != response.headers.end())
            content_length = static_cast<size_t>(std::stoul(it->second));

        while(response.body.size() < content_length)
        {
            ssize_t n = ::recv(_sock, buffer, sizeof(buffer), 0);
            if(n < 0)
            {
                if(errno == EINTR)
                    continue;
                fail(std::string("recv body failed: ") + std::strerror(errno));
            }
            if(n == 0)
                break;
            response.body.append(buffer, buffer + n);
        }

        return response;
    }

    Response request(const std::string& request)
    {
        send_all(request);
        return read_response();
    }

    int recv_one(char* buffer, size_t size)
    {
        ssize_t n = ::recv(_sock, buffer, size, 0);
        if(n < 0)
            return -errno;
        return static_cast<int>(n);
    }

private:
    int _sock = -1;
};

inline std::string basic_get(const std::string& path, const std::string& connection = "close")
{
    return "GET " + path + " HTTP/1.1\r\nHost: " + kHost + "\r\nConnection: " + connection + "\r\n\r\n";
}

inline std::string bad_request()
{
    return "BAD / HTTP/1.1\r\nHost: " + std::string(kHost) + "\r\n\r\n";
}

inline std::filesystem::path large_fixture_path()
{
    return std::filesystem::path("..") / "src" / "wwwroot" / "large.bin";
}

inline void create_large_fixture(std::size_t bytes = 2 * 1024 * 1024)
{
    auto path = large_fixture_path();
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    require(out.is_open(), "failed to create large fixture");
    std::string block(64 * 1024, 'A');
    for(std::size_t offset = 0; offset < bytes; offset += block.size())
    {
        std::size_t write_size = std::min(block.size(), bytes - offset);
        out.write(block.data(), static_cast<std::streamsize>(write_size));
    }
}

inline void remove_large_fixture()
{
    std::error_code ec;
    std::filesystem::remove(large_fixture_path(), ec);
}
}