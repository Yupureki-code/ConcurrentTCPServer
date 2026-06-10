#pragma once

#include "timewheel.h"
#include <memory>
#include <unordered_map>

class EventLoop;

class MultiLevelTimeWheel
{
public:
    MultiLevelTimeWheel(EventLoop* loop);
    ~MultiLevelTimeWheel();
    
    bool AddTimeTask(size_t id, size_t timeout_sec, TimeOutCallBack cb);
    void DeleteTimeTask(size_t id);
    void RefreshTimeTask(size_t id);
    void EnableTimeTask(size_t id);
    void UnableTimeTask(size_t id);
    bool IsTimeTaskExists(size_t id);
    
private:
    void RunOneTime();
    
    TimeWheel _second_wheel;
    TimeWheel _minute_wheel;
    TimeWheel _hour_wheel;
    
    EventLoop* _loop;
    int _tick = 0;
    std::unordered_map<size_t, size_t> _task_level;
};
