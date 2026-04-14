#include "../include/ConcurrentTCPServer.hpp"
#include <string>

void ConcurrentTCPServer::NewConnection(int fd, const InetAddr& peer)
{
    logger(ns_log::DEBUG)<<"fd:"<<std::to_string(fd)<<",peer:"<<peer.get_string();
    PtrConnection con = std::make_shared<Connection>(_next_id, _looppool.SmartChooseThread(), fd, peer);
    logger(DEBUG)<<"con 构造完成";
    con->SetStatus(ConStatus::Connecting);
    con->SetConnectedCallBack(_connected_cb);
    con->SetMessageCallBack(_message_cb);
    con->SetEventCallBack(_event_cb);
    con->SetCloseCallBack(_close_cb);
    con->SetCloseSvrCallBack(std::bind(&ConcurrentTCPServer::RemoveConnection, this, std::placeholders::_1));
    con->Init();
    if (_is_enable_inactive_release)
        con->EnableInactiveRelease(_timeout);
    _cons[_next_id++] = con;
}

void ConcurrentTCPServer::AddTimeTaskInLoop(TimeOutCallBack cb, size_t delay)
{
    _looppool.SmartChooseThread()->AddTimeTask(_next_id++, delay, cb);
}

void ConcurrentTCPServer::RemoveConnectionInLoop(const PtrConnection& con)
{
    auto it = _cons.find(con->GetId());
    if (it != _cons.end())
        _cons.erase(con->GetId());
}

void ConcurrentTCPServer::RemoveConnection(const PtrConnection& con)
{
    _baseloop.RunInLoop(std::bind(&ConcurrentTCPServer::RemoveConnectionInLoop, this, con));
}

ConcurrentTCPServer::ConcurrentTCPServer(uint16_t port, size_t thread_nums)
    : _baseloop()
    , _acceptor(&_baseloop, port, std::bind(&ConcurrentTCPServer::NewConnection, this, std::placeholders::_1, std::placeholders::_2))
    , _port(port)
    , _looppool(thread_nums)
    ,_is_enable_inactive_release(false)
{
    _looppool.SetBaseLoop(&_baseloop);
}

void ConcurrentTCPServer::SetConnectedCallBack(ConnectedCallBack cb) { _connected_cb = cb; }
void ConcurrentTCPServer::SetMessageCallBack(MessageCallBack cb) { _message_cb = cb; }
void ConcurrentTCPServer::SetEventCallBack(EventCallBack cb) { _event_cb = cb; }
void ConcurrentTCPServer::SetCloseCallBack(CloseCallBack cb) { _close_cb = cb; }

void ConcurrentTCPServer::EnableInactiveRelease(size_t timeout)
{
    _is_enable_inactive_release = true;
    _timeout = timeout;
}

void ConcurrentTCPServer::AddTimeTask(TimeOutCallBack cb, size_t delay)
{
    _baseloop.RunInLoop(std::bind(&ConcurrentTCPServer::AddTimeTaskInLoop, this, cb, delay));
}

void ConcurrentTCPServer::Run()
{
    _baseloop.Run();
}