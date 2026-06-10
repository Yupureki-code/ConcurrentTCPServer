#include "../include/EchoServer.hpp"
#include <CTS/CTS.h>
#include <CTS/connection.h>
#include <CTS/buffer.h>
#include <memory>

class EchoServer::Impl
{
public:
    Impl(uint16_t port, size_t thread_nums)
        : _server(port, thread_nums)
    {
        _server.SetMessageCallBack([this](const auto& conn, Buffer& buf) {
            while (buf.Size() > 0) {
                std::string data = buf.GetLine("\n");
                if (data.empty()) break;
                conn->Send(data + "\n");
            }
        });

        _server.SetConnectedCallBack([](const auto& conn) {
        });

        _server.SetCloseCallBack([](const auto& conn) {
        });

        _server.EnableInactiveRelease(60);
    }

    void Start()
    {
        _server.Run();
    }

private:
    CTS _server;
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