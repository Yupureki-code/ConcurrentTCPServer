#include "../include/EchoServer.hpp"
#include "../include/ConcurrentTCPServer.hpp"
#include "../include/connection.h"
#include "../include/buffer.hpp"
#include <iostream>
#include <memory>

class EchoServer::Impl
{
public:
    Impl(uint16_t port, size_t thread_nums)
        : _server(port, thread_nums)
    {
        _server.SetMessageCallBack([this](const auto& conn, Buffer& buf) {
            std::string data = buf.GetLine();
            if (!data.empty())
            {
                std::cout << "EchoServer received: " << data << std::endl;
                conn->Send("Echo: " + data + "\n");
            }
        });

        _server.SetConnectedCallBack([](const auto& conn) {
            std::cout << "Client connected: " << conn->GetId() << std::endl;
        });

        _server.SetCloseCallBack([](const auto& conn) {
            std::cout << "Client disconnected: " << conn->GetId() << std::endl;
        });

        _server.EnableInactiveRelease(60);
    }

    void Start()
    {
        std::cout << "EchoServer starting on port..." << std::endl;
        _server.Run();
    }

private:
    ConcurrentTCPServer _server;
};

EchoServer::EchoServer(uint16_t port, size_t thread_nums)
    : _impl(std::make_unique<Impl>(port, thread_nums))
{
}

EchoServer::~EchoServer() = default;

void EchoServer::SetConnectedCallBack(ConnectedCallBack cb)
{
}

void EchoServer::SetMessageCallBack(MessageCallBack cb)
{
}

void EchoServer::SetCloseCallBack(CloseCallBack cb)
{
}

void EchoServer::Start()
{
    _impl->Start();
}