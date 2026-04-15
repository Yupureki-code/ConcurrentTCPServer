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
    void SetConnectedCallBack(ConnectedCallBack cb);
    void SetMessageCallBack(MessageCallBack cb);
    void SetEventCallBack(EventCallBack cb);
    void SetCloseCallBack(CloseCallBack cb);
    void SetCloseSvrCallBack(CloseCallBack cb);
    uint64_t GetId()const;
    int GetFd()const;
    Context GetContext()const;
    ConStatus GetStatus()const;
    InetAddr GetPeerAddr()const;
    void SetStatus(ConStatus status);
    void EnableInactiveRelease(size_t sec);
    void UnableInactiveRelease();
    void SetContext(Context& context);
    void ShutDown();
    void Send(const std::string& info);
    void Release();
    void Init();
    void Update(Context context,ConnectedCallBack connect_cb,MessageCallBack message_cb,EventCallBack event_cb,CloseCallBack close_cb);
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