#pragma once

#include "InetAddr.hpp"
#include "comm.hpp"
#include <functional>
#include "eventloop.h"
#include "socket.hpp"
#include "channel.h"
#include "eventloop.h"

class Acceptor
{
private:
    using AcceptCallBack = std::function<void(int,const InetAddr&)>;
    void HandlerRead()
    {
        InetAddr peer;
        int fd = _socket->Accept(peer);
        if(fd < 0)
            return;
        _accept_cb(fd,peer);
    }
public:
    Acceptor(EventLoop* loop,uint16_t port,AcceptCallBack cb)
    :_loop(loop),_channel(-1,nullptr),_accept_cb(cb)
    {
        _socket = new TcpSocket;
        _socket->InitTcpServer(port);
        _channel.SetFd(_socket->get_sockfd());
        _channel.SetLoop(loop);
        _channel.SetReadCallBack(std::bind(&Acceptor::HandlerRead,this));
        _channel.EnableRead();
    }
    ~Acceptor()
    {
        delete _socket;
    }
private:
    Socket* _socket;
    Channel _channel;
    EventLoop* _loop;
    AcceptCallBack _accept_cb;
};