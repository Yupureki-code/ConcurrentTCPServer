#include <iostream>
#include <queue>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include <sys/timerfd.h>

using TimeOutCallBack = std::function<void()>;
using ReleaseCallBack = std::function<void()>;

class TimeTask
{
public:
    TimeTask(size_t id,int timeout,TimeOutCallBack timeout_cb,ReleaseCallBack release_cb)
    :_id(id),_timeout(timeout),_timeout_cb(timeout_cb),_release_cb(release_cb)
    {}
    size_t delay_time()
    {
        return _timeout;
    }
    ~TimeTask()
    {
        if(_release_cb)
            _release_cb();
        if(_timeout_cb)
            _timeout_cb();
    }
private:
    size_t _id;
    size_t _timeout;
    TimeOutCallBack _timeout_cb;
    ReleaseCallBack _release_cb;   
};

class TimeWheel
{
private:
    using SharedTimeTask = std::shared_ptr<TimeTask>;
    using WeakTimeTask = std::weak_ptr<TimeTask>;
    static void RemoveWeakTask(TimeWheel* ptr,size_t id)
    {
        ptr->RemoveWeakTaskHelper(id);
    }
    void RemoveWeakTaskHelper(size_t id)
    {
        auto it = _timers.find(id);
        if(it == _timers.end())
            return;
        _timers.erase(it);
    }
public:
    TimeWheel(int capacity)
    :_capacity(capacity),_timewheel(capacity)
    {}
    bool AddTimeTask(size_t id,size_t timeout,TimeOutCallBack cb)
    {
        if(timeout > _capacity)
            return false;
        SharedTimeTask stt = std::make_shared<TimeTask>(id,timeout,cb,std::bind(RemoveWeakTask,this,id));
        _timewheel[(_tick + timeout) % _capacity].push(stt);
        WeakTimeTask wtt = stt;
        _timers[id] = wtt;
        return true;
    }
    void RefreshTimeTask(size_t id)
    {
        auto it = _timers.find(id);
        if(it == _timers.end())
            return;
        SharedTimeTask ptr = (*it).second.lock();
        _timewheel[(ptr->delay_time() + _tick) % _capacity].push(ptr);
    }
    void RunOneTime()
    {
        _tick = (_tick + 1)%_capacity;
        auto& q = _timewheel[_tick];
        while(!q.empty())
        {
            auto task = std::move(q.front());
            q.pop();
        }
    }
    size_t GetTick() const { return _tick; }
private:
    std::vector<std::queue<SharedTimeTask>> _timewheel;
    std::unordered_map<size_t,WeakTimeTask> _timers;
    size_t _capacity;
    int _tick = 0;
};