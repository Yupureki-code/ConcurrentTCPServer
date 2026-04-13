#pragma once

#include "comm.hpp"
#include "eventloop.h"

class Poll;

class Channel
{
private:
    using EventCallBack = std::function<void()>;
public: 
    Channel(int fd,EventLoop* loop = nullptr)
    :_fd(fd),_loop(loop),_events(0),_revents(0)
    {}
    int GetFd(){return _fd;}
    void SetFd(int fd){_fd = fd;}
    void SetLoop(EventLoop* loop){_loop = loop;}
    uint32_t GetEvents(){return _events;}
    void SetREvents(uint32_t events){_revents = events;}
    void SetEventCallBack(const EventCallBack& cb){ _event = cb;}
    void SetReadCallBack(const EventCallBack& cb){ _read = cb;}
    void SetWriteCallBack(const EventCallBack& cb){ _write = cb;}
    void SetErrorCallBack(const EventCallBack& cb){ _error = cb;}
    void SetCloseCallBack(const EventCallBack& cb){ _close = cb;}
    bool IsReadAble(){return _events & EPOLLIN;}
    bool IsWriteAble(){return _events & EPOLLOUT;}
    void EnableRead(){_events |= EPOLLIN;logger(ns_log::INFO) << "Channel EnableRead: fd=" << _fd << " events=" << _events;Add();}
    void EnableWrite(){_events |= EPOLLOUT;Add();}
    void UnableRead(){_events &= ~EPOLLIN;Add();}
    void UnableWrite(){_events &= ~EPOLLOUT;Add();}
    void HandlerEvent();
    void Add();
    void Remove();
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
};