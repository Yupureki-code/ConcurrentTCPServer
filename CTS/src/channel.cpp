#include "../include/channel.h"
#include "../include/poll.h"

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
            logger(ns_log::DEBUG)<<"处理EPOLLERR";
            if(_event)_event();
            if(_error)_error();
        }
        if(_revents & EPOLLHUP)
        {
            logger(ns_log::DEBUG)<<"处理EPOLLHUP";
            if(_event)_event();
            if(_close)_close();
        }
        return;
    }
    if((_revents & EPOLLIN) || (_revents & EPOLLRDHUP) || (_revents & EPOLLPRI))
    {
        if(_revents & EPOLLIN)
            logger(ns_log::DEBUG)<<"处理EPOLLIN";
        if(_revents & EPOLLRDHUP)
            logger(ns_log::DEBUG)<<"处理EPOLLRDHUP";
        if(_revents & EPOLLPRI)
            logger(ns_log::DEBUG)<<"处理EPOLLPRI";
        if(_event)_event();
        if(_read)_read();
    }
    if(_revents & EPOLLOUT)
    {
        logger(ns_log::DEBUG)<<"处理EPOLLOUT";
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