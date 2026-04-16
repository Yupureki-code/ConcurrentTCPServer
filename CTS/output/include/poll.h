#pragma once

#include "comm.hpp"

class Channel;

#define MAX_EPOLL_NUM 1024
/// Poll类用于管理文件描述符和事件的Poll对象，提供了添加、删除Channel以及运行事件循环等功能
class Poll
{
private:
    bool ExistCannel(Channel* channel);//判断Channel是否存在
    void EpollCTL(Channel* channel,int oper);//对Channel进行Epoll控制，oper表示操作类型，添加、修改或删除
public:
    Poll(int num = MAX_EPOLL_NUM);
    void AddChannel(Channel* channel);//将Channel添加到Poll中
    void RemoveChannel(Channel* channel);//将Channel从Poll中移除
    void Run(std::vector<Channel*>& actives);//运行事件循环，等待事件发生，并将发生事件的Channel存储在actives中
    int GetPollNums();//获取当前Poll中管理的Channel数量
    int GetEpfd();//获取Poll的文件描述符
private:
    int _epfd;//Poll的文件描述符
    std::vector<struct epoll_event> _events;//用于存储发生事件的epoll_event结构体数组
    std::unordered_map<int, Channel*> _channels;//用于存储文件描述符和Channel的映射关系，方便根据文件描述符找到对应的Channel
};