#pragma once

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
#include "logstrategy.hpp"
#include "socket.hpp"
#include "eventloop.h"
#include "buffer.hpp"

enum ConStatus{
    Connecting,
    Connected,
    Disconnecting,
    Disconnected
};

class Connection : public std::enable_shared_from_this<Connection>
{
private:
    using PtrConnection = std::shared_ptr<Connection>;
    using ConnectedCallBack = std::function<void(const PtrConnection&)>;
    using MessageCallBack = std::function<void(const PtrConnection&,Buffer&)>;
    using EventCallBack = std::function<void(const PtrConnection&)>;
    using CloseCallBack = std::function<void(const PtrConnection&)>;
    void HandlerRead();
    void HandlerWrite();
    void HandlerError();
    void HandlerClose();
    void HandlerEvent();
    void EnableInactiveReleaseInLoop(size_t sec);
    void UnableInactiveReleaseInLoop();
    void UpdateInLoop(Context context,ConnectedCallBack connect_cb,MessageCallBack message_cb,EventCallBack event_cb,CloseCallBack close_cb);
    void InitInLoop();
    void BindChannelCallbacksInLoop();
    void ShutDownInLoop();
    void ReleaseInLoop();
    void SendInLoop(const std::string& info);
public:
    Connection(uint64_t _id,EventLoop* loop,int fd,const InetAddr& peer);
    void SetConnectedCallBack(ConnectedCallBack cb){_connected_cb = cb;}
    void SetMessageCallBack(MessageCallBack cb){_message_cb = cb;}
    void SetEventCallBack(EventCallBack cb){_event_cb = cb;}
    void SetCloseCallBack(CloseCallBack cb){_close_cb = cb;}
    void SetCloseSvrCallBack(CloseCallBack cb){_close_server_cb = cb;}
    uint64_t GetId()const{return _id;}
    int GetFd()const{return _socket->get_sockfd();}
    Context GetContext()const{return _context;}
    ConStatus GetStatus()const{return _status;}
    void SetStatus(ConStatus status){_status = status;}
    void EnableInactiveRelease(size_t sec)
    {
        auto self = shared_from_this();
        _loop->RunInLoop([self, sec]() { self->EnableInactiveReleaseInLoop(sec); });
    }
    void UnableInactiveRelease()
    {
        auto self = shared_from_this();
        _loop->RunInLoop([self]() { self->UnableInactiveReleaseInLoop(); });
    };
    void SetContext(Context& context){_context = context;}
    void ShutDown()
    {
        auto self = shared_from_this();
        _loop->RunInLoop([self]() { self->ShutDownInLoop(); });
    }
    void Send(const std::string& info)
    {
        auto self = shared_from_this();
        _loop->RunInLoop([self, info]() { self->SendInLoop(info); });
    };
    void Release()
    {
        auto self = shared_from_this();
        _loop->RunInLoop([self]() { self->ReleaseInLoop(); });
    };
    void Init()
    {
        logger(ns_log::DEBUG)<<"进入init";
        assert(_loop != nullptr);
        auto self = shared_from_this();
        _loop->RunInLoop([self]() { self->InitInLoop(); });
    };
    void Update(Context context,ConnectedCallBack connect_cb,MessageCallBack message_cb,EventCallBack event_cb,CloseCallBack close_cb)
    {
        assert(_loop->IsInLoop());
        auto self = shared_from_this();
        _loop->PushInLoop([self, context, connect_cb, message_cb, event_cb, close_cb]() {
            self->UpdateInLoop(context, connect_cb, message_cb, event_cb, close_cb);
        });
    }
private:
    uint64_t _id;
    Context _context;
    std::unique_ptr<Socket> _socket;
    InetAddr _peer;
    EventLoop* _loop;
    Channel _channel;
    Buffer _in_buffer;
    Buffer _out_buffer;
    ConStatus _status = Disconnected;
    bool _enable_inactive_release = false;
    ConnectedCallBack _connected_cb;
    MessageCallBack _message_cb;
    EventCallBack _event_cb;
    CloseCallBack _close_cb;
    CloseCallBack _close_server_cb;
};