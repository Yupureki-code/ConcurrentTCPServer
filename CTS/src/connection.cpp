#include "../include/connection.h"
#include <cerrno>
#include <cassert>
#include <memory>

void Connection::SetConnectedCallBack(ConnectedCallBack cb){ _connected_cb = cb; }
void Connection::SetMessageCallBack(MessageCallBack cb){ _message_cb = cb; }
void Connection::SetEventCallBack(EventCallBack cb){ _event_cb = cb; }
void Connection::SetCloseCallBack(CloseCallBack cb){ _close_cb = cb; }
void Connection::SetCloseSvrCallBack(CloseCallBack cb){ _close_server_cb = cb; }
uint64_t Connection::GetId()const{ return _id; }
int Connection::GetFd()const{ return _socket->get_sockfd(); }
Context Connection::GetContext()const{ return _context; }
InetAddr Connection::GetPeerAddr()const{ return _peer; }
ConStatus Connection::GetStatus()const{ return _status; }
void Connection::SetStatus(ConStatus status){ _status = status; }
void Connection::EnableInactiveRelease(size_t sec)
{
    auto self = shared_from_this();
    _loop->RunInLoop([self, sec]() { self->EnableInactiveReleaseInLoop(sec); });
}
void Connection::UnableInactiveRelease()
{
    auto self = shared_from_this();
    _loop->RunInLoop([self]() { self->UnableInactiveReleaseInLoop(); });
}
void Connection::SetContext(Context& context){ _context = context; }
void Connection::ShutDown()
{
    auto self = shared_from_this();
    _loop->RunInLoop([self]() { self->ShutDownInLoop(); });
}
void Connection::Send(const std::string& info)
{
    auto self = shared_from_this();
    _loop->RunInLoop([self, info]() { self->SendInLoop(info); });
}
void Connection::Release()
{
    auto self = shared_from_this();
    _loop->RunInLoop([self]() { self->ReleaseInLoop(); });
}
void Connection::Init()
{
    logger(ns_log::DEBUG) << "进入init";
    assert(_loop != nullptr);
    auto self = shared_from_this();
    _loop->RunInLoop([self]() { self->InitInLoop(); });
}
void Connection::Update(Context context,ConnectedCallBack connect_cb,MessageCallBack message_cb,EventCallBack event_cb,CloseCallBack close_cb)
{
    assert(_loop->IsInLoop());
    auto self = shared_from_this();
    _loop->PushInLoop([self, context, connect_cb, message_cb, event_cb, close_cb]() {
        self->UpdateInLoop(context, connect_cb, message_cb, event_cb, close_cb);
    });
}

void Connection::HandlerRead()
{
    std::string in;
    int n = _socket->NonBlockRecv(in);
    logger(ns_log::DEBUG) << "received:" << in << ", count:" << n;
    if(n == 0)
        return;
    if(n == -1)
    {
        ShutDown();
        return;
    }
    if(n < -1)
    {
        Release();
        return;
    }
    _in_buffer.Write(in);
    if(_in_buffer.Size())
        _message_cb(shared_from_this(),_in_buffer);
}
void Connection::HandlerWrite()
{
    std::string out = _out_buffer.Read();
    if(out.empty())
    {
        _channel.UnableWrite();
        return;
    }
    int n = _socket->NonBlockSend(out);
    if(n < 0)
    {
        _out_buffer.Write(out);
        if(errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
            return;
        if(_in_buffer.Size())
            _message_cb(shared_from_this(),_in_buffer);
        Release();
        return;
    }
    if(static_cast<size_t>(n) < out.size())
    {
        _out_buffer.Write(out.substr(n));
        _channel.EnableWrite();
        return;
    }
    if(_out_buffer.Size() == 0)
    {
        _channel.UnableWrite();
        if(_status == Disconnecting)
            Release();
    }
}
void Connection::HandlerError()
{
    if(_in_buffer.Size())
        _message_cb(shared_from_this(),_in_buffer);
    Release();
}
void Connection::HandlerClose()
{
    HandlerError();
}
void Connection::HandlerEvent()
{
    if(_enable_inactive_release)
        _loop->RefreshTimeTask(_id);
    if(_event_cb)
        _event_cb(shared_from_this());
}

void Connection::EnableInactiveReleaseInLoop(size_t sec)
{
    _enable_inactive_release = true;
    std::weak_ptr<Connection> weak_self = shared_from_this();
    _loop->AddTimeTask(_id, sec, [weak_self]() {
        auto self = weak_self.lock();
        if(self)
            self->Release();
    });
}

void Connection::UnableInactiveReleaseInLoop()
{
    _enable_inactive_release = false;
    _loop->UnableTimeTask(_id);
}

void Connection::UpdateInLoop(Context context,ConnectedCallBack connect_cb,MessageCallBack message_cb,EventCallBack event_cb,CloseCallBack close_cb)
{
    _context = context;
    _connected_cb = connect_cb;
    _message_cb = message_cb;
    _close_cb = close_cb;
    _event_cb = event_cb;
}

void Connection::InitInLoop()
{
    logger(ns_log::DEBUG)<<"进入InitInLoop";
    if(_status != ConStatus::Connecting)
    {
        logger(ns_log::WARNING) << "skip InitInLoop, status=" << _status;
        return;
    }
    BindChannelCallbacksInLoop();
    _status = Connected;
    _channel.EnableRead();
    if(_connected_cb)
        _connected_cb(shared_from_this());
}

void Connection::BindChannelCallbacksInLoop()
{
    _channel.Tie(shared_from_this());
    std::weak_ptr<Connection> weak_self = shared_from_this();
    _channel.SetReadCallBack([weak_self]() {
        auto self = weak_self.lock();
        if(self)
            self->HandlerRead();
    });
    _channel.SetWriteCallBack([weak_self]() {
        auto self = weak_self.lock();
        if(self)
            self->HandlerWrite();
    });
    _channel.SetCloseCallBack([weak_self]() {
        auto self = weak_self.lock();
        if(self)
            self->HandlerClose();
    });
    _channel.SetErrorCallBack([weak_self]() {
        auto self = weak_self.lock();
        if(self)
            self->HandlerError();
    });
    _channel.SetEventCallBack([weak_self]() {
        auto self = weak_self.lock();
        if(self)
            self->HandlerEvent();
    });
}

void Connection::ShutDownInLoop()
{
    _status = Disconnecting;
    if(_out_buffer.Size())
        _channel.EnableWrite();
    else
        Release();
}

void Connection::ReleaseInLoop()
{
    auto self = shared_from_this();
    _status = Disconnected;
    _channel.Remove();
    _socket->Close();
    _loop->UnableTimeTask(_id);
    _loop->DeleteTimeTask(_id);
    if(_close_cb)
        _close_cb(self);
    if(_close_server_cb)
        _close_server_cb(self);
}

void Connection::SendInLoop(const std::string& info)
{
    if(_status != Connected)
        return;
    _out_buffer.Write(info);
    if(!_channel.IsWriteAble())
        _channel.EnableWrite();
}

Connection::Connection(uint64_t id,EventLoop* loop,int fd,const InetAddr& peer)
:_id(id),_loop(loop),_channel(fd,loop),_peer(peer)
{
    _socket = std::make_unique<TcpSocket>(fd);
}
