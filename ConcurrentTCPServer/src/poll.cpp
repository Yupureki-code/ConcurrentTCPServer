#include "../include/poll.h"
#include "../include/channel.h"

using namespace ns_log;

Poll::Poll(int num)
    :_events(num > 0 ? num : MAX_EPOLL_NUM)
{
    _epfd = epoll_create1(0);
    logger(ns_log::INFO) << "Poll created: epfd=" << _epfd;
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
    memset(&ev, 0, sizeof(ev));
    ev.events = channel->GetEvents();
    ev.data.fd = channel->GetFd();
    int fd = channel->GetFd();
    uint32_t events = channel->GetEvents();
    
    logger(ns_log::INFO) << "EpollCTL: fd=" << fd << " events=" << events << " oper=" << oper << " epfd=" << _epfd;
    
    int flags = fcntl(fd, F_GETFL);
    logger(ns_log::INFO) << "  fd flags=" << flags;
    
    struct epoll_event test_ev;
    memset(&test_ev, 0, sizeof(test_ev));
    test_ev.events = EPOLLIN;
    test_ev.data.fd = fd;
    
    int n = epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &test_ev);
    if(n < 0)
    {
        logger(ns_log::WARNING) << "epoll ctl error (non-fatal): fd=" << fd << " oper=" << oper << " errno=" << errno << " " << strerror(errno);
        return;
    }
    
    logger(ns_log::INFO) << "  Success with EPOLLIN directly";
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