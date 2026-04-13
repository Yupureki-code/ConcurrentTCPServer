#include "../include/channel.h"
#include "../include/poll.h"

void Channel::HandlerEvent()
{
    if(_revents & (EPOLLERR | EPOLLHUP))
    {
        if(_revents & EPOLLERR)
        {
            if(_event)_event();
            if(_error)_error();
        }
        if(_revents & EPOLLHUP)
        {
            if(_event)_event();
            if(_close)_close();
        }
        return;
    }
    if((_revents & EPOLLIN) || (_revents & EPOLLRDHUP) || (_revents & EPOLLPRI))
    {
        if(_read)_read();
        if(_event)_event();
    }
    if(_revents & EPOLLOUT)
    {
        if(_write)_write();
        if(_event)_event();
    }
}

void Channel::Add()
{
    if(_loop)
        _loop->AddChannel(this);
}

void Channel::Remove()
{
    if(_loop)
        _loop->RemoveChannel(this);
}