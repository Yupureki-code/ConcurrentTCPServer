#pragma once

#include <functional>
#include "eventloop.h"
#include "channel.h"
#include "eventloop.h"
#include "InetAddr.h"

class Socket;

class Acceptor
{
private:
    using AcceptCallBack = std::function<void(int,const InetAddr&)>;
    void HandlerRead();
public:
    Acceptor(EventLoop* loop,uint16_t port,AcceptCallBack cb);
private:
    Socket* _socket;
    Channel _channel;
    EventLoop* _loop;
    AcceptCallBack _accept_cb;
};