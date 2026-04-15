#include "../include/ConcurrentTCPServer.hpp"
#include <string>

void CTS::NewConnection(int fd, const InetAddr& peer)
{
    logger(ns_log::DEBUG)<<"fd:"<<std::to_string(fd)<<",peer:"<<peer.get_string();
    PtrConnection con = std::make_shared<Connection>(_next_id, _looppool.SmartChooseThread(), fd, peer);
    logger(ns_log::DEBUG)<<"con 构造完成";
    con->SetStatus(ConStatus::Connecting);
    con->SetConnectedCallBack(_connected_cb);
    con->SetMessageCallBack(_message_cb);
    con->SetEventCallBack(_event_cb);
    con->SetCloseCallBack(_close_cb);
    con->SetCloseSvrCallBack(std::bind(&CTS::RemoveConnection, this, std::placeholders::_1));
    con->Init();
    if (_is_enable_inactive_release)
        con->EnableInactiveRelease(_timeout);
    _cons[_next_id++] = con;
}

void CTS::AddTimeTaskInLoop(TimeOutCallBack cb, size_t delay)
{
    _looppool.SmartChooseThread()->AddTimeTask(_next_id++, delay, cb);
}

void CTS::RemoveConnectionInLoop(const PtrConnection& con)
{
    auto it = _cons.find(con->GetId());
    if (it != _cons.end())
        _cons.erase(con->GetId());
}

void CTS::RemoveConnection(const PtrConnection& con)
{
    _baseloop.RunInLoop(std::bind(&CTS::RemoveConnectionInLoop, this, con));
}

CTS::CTS(uint16_t port, size_t thread_nums)
    : _baseloop()
    , _acceptor(&_baseloop, port, std::bind(&CTS::NewConnection, this, std::placeholders::_1, std::placeholders::_2))
    , _port(port)
    , _looppool(thread_nums)
    ,_is_enable_inactive_release(false)
{
    _looppool.SetBaseLoop(&_baseloop);
}

void CTS::SetConnectedCallBack(ConnectedCallBack cb) { _connected_cb = cb; }
void CTS::SetMessageCallBack(MessageCallBack cb) { _message_cb = cb; }
void CTS::SetEventCallBack(EventCallBack cb) { _event_cb = cb; }
void CTS::SetCloseCallBack(CloseCallBack cb) { _close_cb = cb; }

void CTS::EnableInactiveRelease(size_t timeout)
{
    _is_enable_inactive_release = true;
    _timeout = timeout;
}

void CTS::AddTimeTask(TimeOutCallBack cb, size_t delay)
{
    _baseloop.RunInLoop(std::bind(&CTS::AddTimeTaskInLoop, this, cb, delay));
}

void CTS::Run()
{
    _baseloop.Run();
}