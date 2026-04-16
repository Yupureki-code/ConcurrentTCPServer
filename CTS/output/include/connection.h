#pragma once

#include "InetAddr.h"
#include "InetAddr.hpp"
#include "channel.h"
#include "comm.hpp"
#include <algorithm>
#include <cassert>
#include <functional>
#include <memory>
#include <sys/epoll.h>
#include <system_error>
#include <any>
#include <Logger/logstrategy.h>
#include "socket.hpp"
#include "eventloop.h"
#include "buffer.hpp"

enum ConStatus{
    Connecting,
    Connected,
    Disconnecting,
    Disconnected
};

/// Connection类用于表示一个TCP连接，封装了连接的状态、读写缓冲区、事件处理逻辑等，并提供了连接、消息、事件和关闭的回调接口，以及非活动释放功能
class Connection : public std::enable_shared_from_this<Connection>
{
private:
    using PtrConnection = std::shared_ptr<Connection>;//连接的智能指针类型
    using ConnectedCallBack = std::function<void(const PtrConnection&)>;//连接回调函数类型
    using MessageCallBack = std::function<void(const PtrConnection&,Buffer&)>;//消息回调函数类型
    using EventCallBack = std::function<void(const PtrConnection&)>;//事件回调函数类型
    using CloseCallBack = std::function<void(const PtrConnection&)>;//关闭回调函数类型
    void HandlerRead();//处理读事件的函数
    void HandlerWrite();//处理写事件的函数
    void HandlerError();//处理错误事件的函数
    void HandlerClose();//处理关闭事件的函数
    void HandlerEvent();//处理其他事件的函数
    void EnableInactiveReleaseInLoop(size_t sec);//在事件循环中启用非活动释放功能的函数
    void UnableInactiveReleaseInLoop();//在事件循环中禁用非活动释放功能的函数
    void UpdateInLoop(Context context,ConnectedCallBack connect_cb,MessageCallBack message_cb,EventCallBack event_cb,CloseCallBack close_cb);//在事件循环中更新连接的函数
    void InitInLoop();//在事件循环中初始化连接的函数
    void BindChannelCallbacksInLoop();//在事件循环中绑定通道回调函数的函数
    void ShutDownInLoop();//在事件循环中关闭连接的函数
    void ReleaseInLoop();//在事件循环中释放连接的函数
    void SendInLoop(const std::string& info);//在事件循环中发送数据的函数
public:
    Connection(uint64_t _id,EventLoop* loop,int fd,const InetAddr& peer);//构造函数，参数为连接ID、事件循环、文件描述符和对端地址
    void SetConnectedCallBack(ConnectedCallBack cb);//设置连接回调函数
    void SetMessageCallBack(MessageCallBack cb);//设置消息回调函数
    void SetEventCallBack(EventCallBack cb);//设置事件回调函数
    void SetCloseCallBack(CloseCallBack cb);//设置关闭回调函数
    void SetCloseSvrCallBack(CloseCallBack cb);//设置服务器关闭回调函数
    uint64_t GetId()const;//获取连接ID的函数
    int GetFd()const;//获取连接文件描述符的函数
    Context GetContext()const;//获取连接上下文的函数
    ConStatus GetStatus()const;//获取连接状态的函数
    InetAddr GetPeerAddr()const;//获取连接对端地址的函数
    void SetStatus(ConStatus status);//设置连接状态的函数
    void EnableInactiveRelease(size_t sec);//启用连接的非活动释放功能的函数，参数为超时时间
    void UnableInactiveRelease();//禁用连接的非活动释放功能的函数
    void SetContext(Context& context);//设置连接上下文的函数
    void ShutDown();//关闭连接的函数
    void Send(const std::string& info);//发送数据的函数
    void Release();//释放连接的函数
    void Init();//初始化连接的函数
    void Update(Context context,ConnectedCallBack connect_cb,MessageCallBack message_cb,EventCallBack event_cb,CloseCallBack close_cb);//更新连接的函数
private:
    uint64_t _id;//连接ID
    Context _context;//连接上下文
    std::unique_ptr<Socket> _socket;//连接的套接字
    InetAddr _peer;//连接的对端地址
    EventLoop* _loop;//事件循环
    Channel _channel;//通道
    Buffer _in_buffer;//输入缓冲区
    Buffer _out_buffer;//输出缓冲区
    ConStatus _status = Disconnected;//连接状态
    bool _enable_inactive_release = false;//是否启用非活动释放
    ConnectedCallBack _connected_cb;//连接回调函数
    MessageCallBack _message_cb;//消息回调函数
    EventCallBack _event_cb;//事件回调函数
    CloseCallBack _close_cb;//关闭回调函数
    CloseCallBack _close_server_cb;//服务器关闭回调函数
};