#pragma once

#include "channel.h"
#include "InetAddr.h"
#include "comm.h"
#include <functional>

class EventLoop;
class Socket;

class Acceptor
{
private:
	using AcceptCallBack = std::function<void(int,const InetAddr&)>;
	void HandlerRead();

public:
	Acceptor(EventLoop* loop, uint16_t port, AcceptCallBack cb);
	~Acceptor();

private:
	Socket* _socket;
	Channel _channel;
	EventLoop* _loop;
	AcceptCallBack _accept_cb;
};
