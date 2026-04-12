#include "../include/poll.h"
#include "../include/channel.h"

using namespace ns_log;

Poll::Poll(int num)
    :_events(num)
{
    _epfd = epoll_create(num);
    if(_epfd < 0)
    {
        logger(FATAL)<<"epoll create error!";
        exit(ExitCode::EPOLL_CREATE);
    }
}

bool Poll::ExistCannel(Channel* channel)
{
    auto it = _channels.find(channel->GetFd());
    if(it == _channels.end())
        return false;
    return true;
}

void Poll::EpollCTL(Channel* channel, int oper)
{
    struct epoll_event ev;
    ev.events = channel->GetEvents();
    ev.data.fd = channel->GetFd();
    int n = epoll_ctl(_epfd, oper, channel->GetFd(), &ev);
    if(n < 0)
    {
        logger(ns_log::FATAL)<<"epoll ctl error!";
        exit(EPOLL_CTL);
    }
}

void Poll::AddChannel(Channel* channel)
{
    if(ExistCannel(channel))
        EpollCTL(channel, EPOLL_CTL_MOD);
    else
    {
        EpollCTL(channel, EPOLL_CTL_ADD);
        _channels[channel->GetFd()] = channel;
    }
}

void Poll::RemoveChannel(Channel* channel)
{
    if(ExistCannel(channel))
    {
        EpollCTL(channel, EPOLL_CTL_DEL);
        _channels.erase(channel->GetFd());
    }
}

void Poll::Run(std::vector<Channel*>& actives)
{
    int n = epoll_wait(_epfd, &_events[0], _events.size(), -1);
    if(n < 0)
    {
        logger(ns_log::FATAL)<<"epoll wait error!";
        exit(ExitCode::EPOLL_WAIT);
    }
    for(int i = 0; i < n; i++)
    {
        int fd = _events[i].data.fd;
        Channel* channel = _channels[fd];
        if(!ExistCannel(channel))
        {
            logger(ns_log::WARNING)<<"not found channel,fd: "<<fd;
            continue;
        }
        channel->SetREvents(_events[i].events);
        actives.push_back(channel);
    }
}