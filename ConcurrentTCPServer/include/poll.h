#pragma once

#include "comm.hpp"

class Channel;

#define MAX_EPOLL_NUM 1024

class Poll
{
private:
    bool ExistCannel(Channel* channel);
    void EpollCTL(Channel* channel,int oper);
public:
    Poll(int num = MAX_EPOLL_NUM);
    void AddChannel(Channel* channel);
    void RemoveChannel(Channel* channel);
    void Run(std::vector<Channel*>& actives);
    int GetPollNums(){return _channels.size();}
    int GetEpfd(){return _epfd;}
private:
    int _epfd;
    std::vector<struct epoll_event> _events;
    std::unordered_map<int, Channel*> _channels;
};