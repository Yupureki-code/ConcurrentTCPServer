#pragma once

#include <string>
#include <functional>
#include <memory>

class Connection;
class Buffer;

class EchoServer
{
public:
    using ConnectedCallBack = std::function<void(const std::shared_ptr<Connection>&)>;
    using MessageCallBack = std::function<void(const std::shared_ptr<Connection>&, Buffer&)>;
    using CloseCallBack = std::function<void(const std::shared_ptr<Connection>&)>;

    EchoServer(uint16_t port, size_t thread_nums = 0);
    ~EchoServer();

    void SetConnectedCallBack(ConnectedCallBack cb);
    void SetMessageCallBack(MessageCallBack cb);
    void SetCloseCallBack(CloseCallBack cb);

    void Start();

private:
    class Impl;
    std::unique_ptr<Impl> _impl;
};