#pragma once

#include "poll.h"
#include "timewheel.h"
#include "lock.h"
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
    bool IsInLoop(){return _thread_id == std::this_thread::get_id();}
    void RunInLoop(const func& cb);
    void PushInLoop(const func& cb);
    void EnableTimeTask(size_t id){_timewheel.EnableTimeTask(id);}
    void UnableTimeTask(size_t id){_timewheel.UnableTimeTask(id);}
    void AddTimeTask(size_t id,size_t timeout,TimeOutCallBack cb){_timewheel.AddTimeTask(id, timeout,cb);}
    void RefreshTimeTask(size_t id){_timewheel.RefreshTimeTask(id);}
    void DeleteTimeTask(size_t id){_timewheel.DeleteTimeTask(id);}
    void AddChannel(Channel * channel){_poll.AddChannel(channel);}
    void RemoveChannel(Channel * channel){_poll.RemoveChannel(channel);}
    int GetConsNums(){return _poll.GetPollNums();}
private:
    std::thread::id _thread_id;
    int _event_fd;
    std::unique_ptr<Channel> _event_channel;
    Poll _poll;
    std::queue<func> _task_queue;
    mylock _mutex;
    TimeWheel _timewheel;
};