#include "../include/poll.h"
#include "../include/channel.h"

using namespace ns_log;

int Poll::GetPollNums(){ return _channels.size(); }
int Poll::GetEpfd(){ return _epfd; }

Poll::Poll(int num)
    :_events(num > 0 ? num : MAX_EPOLL_NUM)
{
    _epfd = epoll_create1(0);
    logger(ns_log::INFO) << "Poll created: epfd=" << _epfd;
    if(_epfd < 0)
    {
        logger(ns_log::FATAL)<<"epoll create error!";
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
    memset(&ev, 0, sizeof(ev));
    ev.events = channel->GetEvents();
    ev.data.fd = channel->GetFd();
    int fd = channel->GetFd();
    uint32_t events = channel->GetEvents();
    
    logger(ns_log::INFO) << "EpollCTL: fd=" << fd << " events=" << events << " oper=" << oper << " epfd=" << _epfd;
    
    int n = epoll_ctl(_epfd, oper, fd, &ev);
    if(n < 0)
    {
        logger(ns_log::WARNING) << "epoll ctl error (non-fatal): fd=" << fd << " oper=" << oper << " errno=" << errno << " " << strerror(errno);
        return;
    }
    
    logger(ns_log::INFO) << "  Success";
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
        auto it = _channels.find(fd);
        if(it == _channels.end() || it->second == nullptr)
        {
            logger(ns_log::WARNING)<<"not found channel,fd: "<<fd;
            continue;
        }
        Channel* channel = it->second;
        channel->SetREvents(_events[i].events);
        actives.push_back(channel);
    }
}
