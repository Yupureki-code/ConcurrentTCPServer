#pragma once

#include <functional>
#include <memory>
#include <queue>

class EventLoop;

#define DEFAULT_TIMEWHEEL_NUM 60

using TimeOutCallBack = std::function<void()>;
using ReleaseCallBack = std::function<void()>;

class TimeTask
{
public:
    TimeTask(size_t id,int timeout,TimeOutCallBack timeout_cb,ReleaseCallBack release_cb)
    :_id(id),_timeout(timeout),_timeout_cb(timeout_cb),_release_cb(release_cb)
    {}
    size_t delay_time(){ return _timeout;}
    ~TimeTask()
    {
        if(_timeout_cb && _is_enable)
            _timeout_cb();
        if(_release_cb)
            _release_cb();
    }
    void Enable(){_is_enable = true;}
    void Unable(){_is_enable = false;}

private:
    size_t _id;
    size_t _timeout;
    bool _is_enable = true;;
    TimeOutCallBack _timeout_cb;
    ReleaseCallBack _release_cb;   
};

class TimeWheel
{
private:
    using SharedTimeTask = std::shared_ptr<TimeTask>;
    using WeakTimeTask = std::weak_ptr<TimeTask>;
    static void RemoveWeakTask(TimeWheel* ptr,size_t id){ptr->RemoveWeakTaskHelper(id);}
    void RemoveWeakTaskHelper(size_t id);
    void EnableTimeTaskInLoop(size_t id);
    void UnableTimeTaskInLoop(size_t id);
    bool AddTimeTaskInLoop(size_t id,size_t timeout,TimeOutCallBack cb);
    void RefreshTimeTaskInLoop(size_t id);
    void DeleteTimeTaskInLoop(size_t id);
public:
    TimeWheel(int capacity = DEFAULT_TIMEWHEEL_NUM,EventLoop* loop = nullptr)
    :_capacity(capacity),_timewheel(capacity),_loop(loop)
    {}
    bool IsTimeTaskExists(size_t id);
    void RunOneTime();
    size_t GetTick() const { return _tick; }
    void EnableTimeTask(size_t id);
    void UnableTimeTask(size_t id);
    void AddTimeTask(size_t id,size_t timeout,TimeOutCallBack cb);
    void RefreshTimeTask(size_t id);
    void DeleteTimeTask(size_t id);
private:
    std::vector<std::queue<SharedTimeTask>> _timewheel;
    std::unordered_map<size_t,WeakTimeTask> _timers;
    size_t _capacity;
    int _tick = 0;
    EventLoop* _loop;
};