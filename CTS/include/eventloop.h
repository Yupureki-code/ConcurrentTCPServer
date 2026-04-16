#pragma once

#include "comm.hpp"
#include "channel.h"
#include "lock.hpp"
#include "poll.h"
#include "timewheel.h"
#include <thread>
#include <queue>
#include <functional>
#include <memory>
#include <sys/eventfd.h>

class Channel;
/// EventLoop类用于管理事件循环，处理文件描述符的事件，执行定时任务，以及提供线程安全的任务队列
class EventLoop//事件循环类
{
private:
    using func = std::function<void()>;//任务类型
    void ReadEvent();//处理eventfd的读事件，主要用于唤醒事件循环
    void WakeUpEvent();//唤醒事件循环，向eventfd写入数据
public:
    EventLoop();
    void Run();//事件循环的主函数，负责等待事件和执行任务
    void RunAllTasks();//执行所有待处理的任务
    bool IsInLoop();//判断当前线程是否是事件循环所在的线程
    void RunInLoop(const func& cb);//在事件循环所在的线程执行任务，如果当前线程就是事件循环所在的线程，则直接执行，否则将任务加入队列
    void PushInLoop(const func& cb);//将任务加入队列，并唤醒事件循环
    void EnableTimeTask(size_t id);//启用定时任务
    void UnableTimeTask(size_t id);//禁用定时任务
    void AddTimeTask(size_t id,size_t timeout,TimeOutCallBack cb);//添加定时任务，指定任务ID、超时时间和回调函数
    void RefreshTimeTask(size_t id);//刷新定时任务，重置定时器
    void DeleteTimeTask(size_t id);//删除定时任务
    void AddChannel(Channel * channel);//将Channel添加到事件循环中
    void RemoveChannel(Channel * channel);//将Channel从事件循环中移除
    int GetConsNums();//获取当前事件循环中管理的连接数量
private:
    std::thread::id _thread_id;//事件循环所在的线程ID
    int _event_fd;//用于唤醒事件循环的文件描述符
    std::unique_ptr<Channel> _event_channel;//用于监听_event_fd的Channel
    Poll _poll;//用于管理文件描述符和事件的Poll对象
    std::queue<func> _task_queue;//任务队列，用于存储需要在事件循环中执行的任务
    mylock _mutex;//保护任务队列的互斥锁
    TimeWheel _timewheel;//定时器对象，用于管理定时任务
};