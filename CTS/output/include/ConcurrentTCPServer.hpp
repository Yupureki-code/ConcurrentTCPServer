#pragma once

#include "comm.hpp"
#include "ThreadLoopPool.h"
#include "connection.h"
#include "acceptor.h"
#include "eventloop.h"
#include "timewheel.h"
#include <memory>
#include <unordered_map>

/// ConcurrentTCPServer类是一个基于事件驱动的高性能TCP服务器，支持多线程处理连接请求和消息，提供了连接、消息、事件和关闭的回调接口，以及定时任务功能
class CTS
{
private:
    using PtrConnection = std::shared_ptr<Connection>;//连接的智能指针类型
    using ConnectedCallBack = std::function<void(const PtrConnection&)>;//连接回调函数类型
    using MessageCallBack = std::function<void(const PtrConnection&, Buffer&)>;//消息回调函数类型
    using EventCallBack = std::function<void(const PtrConnection&)>;//事件回调函数类型
    using CloseCallBack = std::function<void(const PtrConnection&)>;//关闭回调函数类型
    using TimeOutCallBack = std::function<void()>;//定时任务回调函数类型
    void NewConnection(int fd, const InetAddr& peer);//新连接到来时的处理函数
    void AddTimeTaskInLoop(TimeOutCallBack cb, size_t delay);//在事件循环中添加定时任务的函数
    void RemoveConnectionInLoop(const PtrConnection& con);//在事件循环中移除连接的函数
    void RemoveConnection(const PtrConnection& con);//移除连接的函数
public:
    CTS(uint16_t port, size_t thread_nums = 0);//构造函数，参数为监听端口和线程数量
    void SetConnectedCallBack(ConnectedCallBack cb);//设置连接回调函数
    void SetMessageCallBack(MessageCallBack cb);//设置消息回调函数
    void SetEventCallBack(EventCallBack cb);//设置事件回调函数
    void SetCloseCallBack(CloseCallBack cb);//设置关闭回调函数
    void EnableInactiveRelease(size_t timeout);//启用连接的非活动释放功能，参数为超时时间
    void AddTimeTask(TimeOutCallBack cb, size_t delay);//添加定时任务的函数，参数为回调函数和延迟时间
    void Run();//运行服务器的函数
private:
    uint16_t _port;//监听的端口
    uint64_t _next_id = 1;//下一个连接的ID
    size_t _thread_nums;//线程数量
    EventLoop _baseloop;//基础事件循环
    Acceptor _acceptor;//连接接受器
    ThreadLoopPool _looppool;//线程事件循环池
    std::unordered_map<uint64_t, PtrConnection> _cons;//连接ID到连接对象的映射
    bool _is_enable_inactive_release;//是否启用连接的非活动释放功能
    size_t _timeout;//连接的非活动释放超时时间
    ConnectedCallBack _connected_cb;//连接回调函数
    MessageCallBack _message_cb;//消息回调函数
    EventCallBack _event_cb;//事件回调函数
    CloseCallBack _close_cb;//关闭回调函数
};