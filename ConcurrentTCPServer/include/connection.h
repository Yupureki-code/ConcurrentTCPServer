#pragma once

#include "InetAddr.hpp"
#include "channel.h"
#include "comm.hpp"
#include <algorithm>
#include <functional>
#include <memory>
#include <sys/epoll.h>
#include <system_error>
#include <any>
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
    uint64_t GetId(){return _id;}
    int GetFd(){return _socket->get_sockfd();}
    Context GetContext(){return _context;}
    ConStatus GetStatus(){return _status;}
    void EnableInactiveRelease(size_t sec){_loop->RunInLoop(std::bind(&Connection::EnableInactiveReleaseInLoop,this,sec));}
    void UnableInactiveRelease(){_loop->RunInLoop(std::bind(&Connection::UnableInactiveReleaseInLoop,this));};
    void SetContext(Context& context){_context = context;}
    void ShutDown(){_loop->RunInLoop(std::bind(&Connection::ShutDownInLoop,this));}
    void Send(const std::string& info){_loop->RunInLoop(std::bind(&Connection::SendInLoop,this,info));};
    void Release(){_loop->RunInLoop(std::bind(&Connection::ReleaseInLoop,this));};
    void Init(){_loop->RunInLoop(std::bind(&Connection::InitInLoop,this));};
    void Update(Context context,ConnectedCallBack connect_cb,MessageCallBack message_cb,EventCallBack event_cb,CloseCallBack close_cb){assert(_loop->IsInLoop());_loop->PushInLoop(std::bind(&Connection::UpdateInLoop,this,context,connect_cb,message_cb,event_cb,close_cb));}
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
    bool _enable_inactive_release;
    ConnectedCallBack _connected_cb;
    MessageCallBack _message_cb;
    EventCallBack _event_cb;
    CloseCallBack _close_cb;
    CloseCallBack _close_server_cb;
};