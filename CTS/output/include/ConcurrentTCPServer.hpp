#pragma once

#include "comm.hpp"
#include "ThreadLoopPool.h"
#include "connection.h"
#include "acceptor.h"
#include "eventloop.h"
#include "timewheel.h"
#include <memory>
#include <unordered_map>

class CTS
{
private:
    using PtrConnection = std::shared_ptr<Connection>;
    using ConnectedCallBack = std::function<void(const PtrConnection&)>;
    using MessageCallBack = std::function<void(const PtrConnection&, Buffer&)>;
    using EventCallBack = std::function<void(const PtrConnection&)>;
    using CloseCallBack = std::function<void(const PtrConnection&)>;
    using TimeOutCallBack = std::function<void()>;
    void NewConnection(int fd, const InetAddr& peer);
    void AddTimeTaskInLoop(TimeOutCallBack cb, size_t delay);
    void RemoveConnectionInLoop(const PtrConnection& con);
    void RemoveConnection(const PtrConnection& con);
public:
    CTS(uint16_t port, size_t thread_nums = 0);
    void SetConnectedCallBack(ConnectedCallBack cb);
    void SetMessageCallBack(MessageCallBack cb);
    void SetEventCallBack(EventCallBack cb);
    void SetCloseCallBack(CloseCallBack cb);
    void EnableInactiveRelease(size_t timeout);
    void AddTimeTask(TimeOutCallBack cb, size_t delay);
    void Run();
private:
    uint16_t _port;
    uint64_t _next_id = 1;
    size_t _thread_nums;
    EventLoop _baseloop;
    Acceptor _acceptor;
    ThreadLoopPool _looppool;
    std::unordered_map<uint64_t, PtrConnection> _cons;
    bool _is_enable_inactive_release;
    size_t _timeout;
    ConnectedCallBack _connected_cb;
    MessageCallBack _message_cb;
    EventCallBack _event_cb;
    CloseCallBack _close_cb;
};