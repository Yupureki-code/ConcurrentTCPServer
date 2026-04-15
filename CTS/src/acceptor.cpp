#include "../include/acceptor.h"

#include "../include/channel.h"
#include "../include/eventloop.h"
#include "../include/InetAddr.hpp"
#include "../include/socket.h"

Acceptor::Acceptor(EventLoop* loop, uint16_t port, AcceptCallBack cb)
    : _socket(new TcpSocket)
    , _channel(-1, nullptr)
    , _loop(loop)
    , _accept_cb(cb)
{
    _socket->InitTcpServer(port);
    _channel.SetFd(_socket->get_sockfd());
    _channel.SetLoop(loop);
    _channel.SetReadCallBack(std::bind(&Acceptor::HandlerRead, this));
    if (_socket->get_sockfd() >= 0)
        _channel.EnableRead();
}

void Acceptor::HandlerRead()
{
    InetAddr peer;
    int fd = _socket->Accept(peer);
    if (fd < 0)
        return;
    _accept_cb(fd, peer);
}

Acceptor::~Acceptor()
{
    delete _socket;
}