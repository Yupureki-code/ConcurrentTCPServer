#include "../include/channel.h"
#include "../include/eventloop.h"
#include "../include/poll.h"

using namespace ns_log;

Channel::Channel(int fd,EventLoop* loop)
    : _fd(fd), _events(0), _revents(0), _loop(loop)
{}

int Channel::GetFd(){ return _fd; }
void Channel::SetFd(int fd){ _fd = fd; }
void Channel::SetLoop(EventLoop* loop){ _loop = loop; }
uint32_t Channel::GetEvents(){ return _events; }
void Channel::SetREvents(uint32_t events){ _revents = events; }
void Channel::SetEventCallBack(const EventCallBack& cb){ _event = cb; }
void Channel::SetReadCallBack(const EventCallBack& cb){ _read = cb; }
void Channel::SetWriteCallBack(const EventCallBack& cb){ _write = cb; }
void Channel::SetErrorCallBack(const EventCallBack& cb){ _error = cb; }
void Channel::SetCloseCallBack(const EventCallBack& cb){ _close = cb; }
void Channel::Tie(const std::shared_ptr<void>& ptr)
{
    _tie = ptr;
    _is_tied = true;
}
bool Channel::IsReadAble(){ return _events & EPOLLIN; }
bool Channel::IsWriteAble(){ return _events & EPOLLOUT; }
void Channel::EnableRead(){ _events |= EPOLLIN; Add(); }
void Channel::EnableWrite(){ _events |= EPOLLOUT; Add(); }
void Channel::UnableRead(){ _events &= ~EPOLLIN; Add(); }
void Channel::UnableWrite(){ _events &= ~EPOLLOUT; Add(); }

void Channel::HandlerEvent()
{
    if(_is_tied)
    {
        std::shared_ptr<void> guard = _tie.lock();
        if(guard)
            HandlerEventWithGuard();
        return;
    }
    HandlerEventWithGuard();
}

void Channel::HandlerEventWithGuard()
{
    if(_revents & (EPOLLERR | EPOLLHUP))
    {
        if(_revents & EPOLLERR)
        {
            logger(ns_log::DEBUG)<<"处理EPOLLERR"<<",fd:"<<std::to_string(_fd);;
            if(_event)_event();
            if(_error)_error();
        }
        if(_revents & EPOLLHUP)
        {
            logger(ns_log::DEBUG)<<"处理EPOLLHUP"<<",fd:"<<std::to_string(_fd);;
            if(_event)_event();
            if(_close)_close();
        }
        return;
    }
    if((_revents & EPOLLIN) || (_revents & EPOLLRDHUP) || (_revents & EPOLLPRI))
    {
        if(_revents & EPOLLIN)
            //logger(ns_log::DEBUG)<<"处理EPOLLIN"<<",fd:"<<std::to_string(_fd);;
        if(_revents & EPOLLRDHUP)
            logger(ns_log::DEBUG)<<"处理EPOLLRDHUP"<<",fd:"<<std::to_string(_fd);;
        if(_revents & EPOLLPRI)
            logger(ns_log::DEBUG)<<"处理EPOLLPRI"<<",fd:"<<std::to_string(_fd);;
        if(_event)_event();
        if(_read)_read();
    }
    if(_revents & EPOLLOUT)
    {
        logger(ns_log::DEBUG)<<"处理EPOLLOUT"<<",fd:"<<std::to_string(_fd);;
        logger(ns_log::DEBUG)<<",fd:"<<std::to_string(_fd);
        if(_event)_event();
        if(_write)_write();
    }
}

void Channel::Add()
{
    if(_loop && _fd >= 0)
        _loop->AddChannel(this);
}

void Channel::Remove()
{
    if(_loop)
        _loop->RemoveChannel(this);
}