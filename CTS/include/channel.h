#pragma once

#include "comm.hpp"
#include <memory>

class EventLoop;
class Poll;

class Channel
{
private:
    using EventCallBack = std::function<void()>;
public: 
    Channel(int fd,EventLoop* loop = nullptr);
    int GetFd();
    void SetFd(int fd);
    void SetLoop(EventLoop* loop);
    uint32_t GetEvents();
    void SetREvents(uint32_t events);
    void SetEventCallBack(const EventCallBack& cb);
    void SetReadCallBack(const EventCallBack& cb);
    void SetWriteCallBack(const EventCallBack& cb);
    void SetErrorCallBack(const EventCallBack& cb);
    void SetCloseCallBack(const EventCallBack& cb);
    void Tie(const std::shared_ptr<void>& ptr);
    bool IsReadAble();
    bool IsWriteAble();
    void EnableRead();
    void EnableWrite();
    void UnableRead();
    void UnableWrite();
    void HandlerEvent();
    void Add();
    void Remove();
    void HandlerEventWithGuard();
private:
    int _fd;
    uint32_t _events;
    uint32_t _revents;
    EventLoop* _loop;
    EventCallBack _event;
    EventCallBack _read;
    EventCallBack _write;
    EventCallBack _error;
    EventCallBack _close;
    std::weak_ptr<void> _tie;
    bool _is_tied = false;
};