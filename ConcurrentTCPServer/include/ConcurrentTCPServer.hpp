#include "comm.hpp"
#include "ThreadLoopPool.hpp"
#include "connection.h"
#include "acceptor.hpp"
#include "eventloop.h"
#include "timewheel.h"
#include <memory>
#include <unordered_map>

class ConcurrentTCPServer
{
private:
    using PtrConnection = std::shared_ptr<Connection>;
    using ConnectedCallBack = std::function<void(const PtrConnection&)>;
    using MessageCallBack = std::function<void(const PtrConnection&,Buffer&)>;
    using EventCallBack = std::function<void(const PtrConnection&)>;
    using CloseCallBack = std::function<void(const PtrConnection&)>;
    using TimeOutCallBack = std::function<void()>;
    void NewConnection(int fd,const InetAddr& peer)
    {
        PtrConnection con = std::make_shared<Connection>(_next_id,_looppool.SmartChooseThread(),fd,peer);
        con->SetConnectedCallBack(_connected_cb);
        con->SetMessageCallBack(_message_cb);
        con->SetEventCallBack(_event_cb);
        con->SetCloseCallBack(_close_cb);
        if(_is_enable_inactive_release)
            con->EnableInactiveRelease(_timeout);
        con->Init();
        _cons[_next_id++] = con;
    }
    void AddTimeTaskInLoop(TimeOutCallBack cb,size_t delay)
    {
        _looppool.SmartChooseThread()->AddTimeTask(_next_id++, delay, cb);
    }
    void RemoveConnectionInLoop(const PtrConnection& con)
    {
        auto it = _cons.find(con->GetId());
        if(it != _cons.end())
            _cons.erase(con->GetId());
    }
    void RemoveConnection(const PtrConnection& con)
    {
        _baseloop.RunInLoop(std::bind(&ConcurrentTCPServer::RemoveConnectionInLoop,this,con));
    }
public:
    ConcurrentTCPServer(uint16_t port,size_t thread_nums = 0)
    :_acceptor(&_baseloop, port, std::bind(&ConcurrentTCPServer::NewConnection,this,std::placeholders::_1,std::placeholders::_2))
    ,_port(port)
    ,_looppool(thread_nums)
    {}
    void SetConnectedCallBack(ConnectedCallBack cb){_connected_cb = cb;}
    void SetMessageCallBack(MessageCallBack cb){_message_cb = cb;}
    void SetEventCallBack(EventCallBack cb){_event_cb = cb;}
    void SetCloseCallBack(CloseCallBack cb){_close_cb = cb;}
    void EnableInactiveRelease(size_t timeout)
    {
        _is_enable_inactive_release = true;
        _timeout = timeout;
    }
    void AddTimeTask(TimeOutCallBack cb,size_t delay)
    {
        _baseloop.RunInLoop(std::bind(&ConcurrentTCPServer::AddTimeTaskInLoop,this,cb,delay));
    }
    void Run()
    {
        _baseloop.Run();
    }
private:
    uint16_t _port;
    uint64_t _next_id = 1;
    size_t _thread_nums;
    Acceptor _acceptor;
    EventLoop _baseloop;
    ThreadLoopPool _looppool;
    std::unordered_map<uint64_t, PtrConnection> _cons;
    bool _is_enable_inactive_release;
    size_t _timeout;
    ConnectedCallBack _connected_cb;
    MessageCallBack _message_cb;
    EventCallBack _event_cb;
    CloseCallBack _close_cb;
};