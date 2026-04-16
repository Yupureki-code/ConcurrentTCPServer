#pragma once

#include "channel.h"
#include "InetAddr.h"
#include "comm.h"
#include <functional>

class EventLoop;
class Socket;

/// Acceptor类用于监听指定端口的连接请求，并在有新连接时调用回调函数进行处理
class Acceptor
{
private:
	using AcceptCallBack = std::function<void(int,const InetAddr&)>;
	void HandlerRead();

public:
	Acceptor(EventLoop* loop, uint16_t port, AcceptCallBack cb);
	~Acceptor();

private:
	Socket* _socket;//用于监听的套接字
	Channel _channel;//用于监听套接字的事件
	EventLoop* _loop;//事件循环
	AcceptCallBack _accept_cb;//新连接到来时的回调函数
};
