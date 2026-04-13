#include "../include/connection.h"
#include <cassert>
#include <memory>

void Connection::HandlerRead()
{
    std::string in;
    int n = _socket->NonBlockRecv(in);
    if(n < 0)
        ShutDown();
    _in_buffer.Write(in);
    if(_in_buffer.Size())
        _message_cb(shared_from_this(),_in_buffer);
}
void Connection::HandlerWrite()
{
    int n = _socket->NonBlockSend(_out_buffer.Read());
    if(n < 0)
    {
        if(_in_buffer.Size())
            _message_cb(shared_from_this(),_in_buffer);
        Release();
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
    _loop->AddTimeTask(_id, sec, std::bind(&Connection::Release,this));
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
    assert(_status == ConStatus::Connecting);
    _status = Connected;
    _channel.EnableRead();
    if(_connected_cb)
        _connected_cb(shared_from_this());
}

void Connection::ShutDownInLoop()
{
    _status = Disconnecting;
    if(_in_buffer.Size())
        _message_cb(shared_from_this(),_in_buffer);
    if(_out_buffer.Size() && !_channel.IsWriteAble())
        _channel.EnableWrite();
    else
        Release();
}

void Connection::ReleaseInLoop()
{
    _status = Disconnected;
    _channel.Remove();
    _socket->Close();
    _loop->UnableTimeTask(_id);
    _loop->DeleteTimeTask(_id);
    if(_close_server_cb)
        _close_server_cb(shared_from_this());
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
    _channel.SetReadCallBack(std::bind(&Connection::HandlerRead,this));
    _channel.SetWriteCallBack(std::bind(&Connection::HandlerWrite,this));
    _channel.SetCloseCallBack(std::bind(&Connection::HandlerClose,this));
    _channel.SetErrorCallBack(std::bind(&Connection::HandlerError,this));
    _channel.SetEventCallBack(std::bind(&Connection::HandlerEvent,this));
}
