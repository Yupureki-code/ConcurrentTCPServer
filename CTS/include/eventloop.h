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

class EventLoop
{
private:
    using func = std::function<void()>;
    void ReadEvent();
    void WakeUpEvent();
public:
    EventLoop();
    void Run();
    void RunAllTasks();
    bool IsInLoop();
    void RunInLoop(const func& cb);
    void PushInLoop(const func& cb);
    void EnableTimeTask(size_t id);
    void UnableTimeTask(size_t id);
    void AddTimeTask(size_t id,size_t timeout,TimeOutCallBack cb);
    void RefreshTimeTask(size_t id);
    void DeleteTimeTask(size_t id);
    void AddChannel(Channel * channel);
    void RemoveChannel(Channel * channel);
    int GetConsNums();
private:
    std::thread::id _thread_id;
    int _event_fd;
    std::unique_ptr<Channel> _event_channel;
    Poll _poll;
    std::queue<func> _task_queue;
    mylock _mutex;
    TimeWheel _timewheel;
};