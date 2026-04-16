#include "../include/timewheel.h"
#include "../include/eventloop.h"
#include "../include/timewheel.h"
#include <cstdint>

size_t TimeWheel::GetTick() const
{
    return _tick;
}

TimeWheel::TimeWheel(int capacity, EventLoop* loop)
    : _capacity(capacity), _timewheel(capacity), _loop(loop),_time_channel(std::make_unique<Channel>(CreateTimeFd(), loop))
{
    _time_channel->SetReadCallBack([this]() {
        _loop->PushInLoop([this]() {
            RunOneTime();
        });
    });
    _time_channel->EnableRead();
}

void TimeWheel::RemoveWeakTaskHelper(size_t id)
{
    auto it = _timers.find(id);
    if(it == _timers.end())
        return;
    _timers.erase(it);
}

bool TimeWheel::IsTimeTaskExists(size_t id)
{
    auto it = _timers.find(id);
    if(it == _timers.end())
        return false;
    return true;
}

int TimeWheel::CreateTimeFd()
{
    _timefd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if(_timefd < 0)
    {
        logger(ns_log::FATAL) << "timerfd create error!";
        exit(ExitCode::TIMERFD_CREATE);
    }
    struct itimerspec new_value;
    memset(&new_value, 0, sizeof(new_value));
    new_value.it_interval.tv_sec = 1;
    new_value.it_interval.tv_nsec = 0;
    new_value.it_value.tv_sec = 1;
    new_value.it_value.tv_nsec = 0;
    timerfd_settime(_timefd, 0, &new_value, nullptr);
    return _timefd;
}

int TimeWheel::ReadTimeFd()
{
    uint64_t expirations;
    int n = read(_timefd, &expirations, sizeof(expirations));
    if(n < 0)
    {
        logger(ns_log::FATAL) << "timerfd read error!";
        exit(ExitCode::TIMERFD_READ);
    }
    return expirations;
}

bool TimeWheel::AddTimeTaskInLoop(size_t id,size_t timeout,TimeOutCallBack cb)
{
    if(IsTimeTaskExists(id))
    {
        RefreshTimeTaskInLoop(id);
        return true;
    }
    if(timeout > _capacity)
    {
        logger(ns_log::WARNING) << "time task timeout exceeds wheel capacity: id=" << id << " timeout=" << timeout << " capacity=" << _capacity;
        return false;
    }
    SharedTimeTask stt = std::make_shared<TimeTask>(id,timeout,cb,std::bind(RemoveWeakTask,this,id));
    _timewheel[(_tick + timeout) % _capacity].push(stt);
    WeakTimeTask wtt = stt;
    _timers[id] = wtt;
    return true;
}

void TimeWheel::RefreshTimeTaskInLoop(size_t id)
{
    auto it = _timers.find(id);
    if(it == _timers.end())
        return;
    SharedTimeTask ptr = (*it).second.lock();
    if(!ptr)
    {
        _timers.erase(it);
        return;
    }
    _timewheel[(ptr->delay_time() + _tick) % _capacity].push(ptr);
}
void TimeWheel::RunOneTime()
{
    int times = ReadTimeFd();
    for(int i = 0; i < times; ++i)
    {
        _tick = (_tick + 1)%_capacity;
        auto& q = _timewheel[_tick];
        while(!q.empty())
        {
            auto task = std::move(q.front());
            q.pop();
        }
    }
}

void TimeWheel::EnableTimeTaskInLoop(size_t id)
{
    auto it = _timers.find(id);
    if(it == _timers.end())
        return;
    SharedTimeTask ptr = (*it).second.lock();
    if(!ptr)
    {
        _timers.erase(it);
        return;
    }
    ptr->Enable();
}

void TimeWheel::UnableTimeTaskInLoop(size_t id)
{
    auto it = _timers.find(id);
    if(it == _timers.end())
        return;
    SharedTimeTask ptr = (*it).second.lock();
    if(!ptr)
    {
        _timers.erase(it);
        return;
    }
    ptr->Unable();
}

void TimeWheel::DeleteTimeTaskInLoop(size_t id)
{
    if(IsTimeTaskExists(id))
    {
        UnableTimeTaskInLoop(id);
        _timers.erase(id);
    }
}

void TimeWheel::EnableTimeTask(size_t id)
{
    _loop->RunInLoop(std::bind(&TimeWheel::EnableTimeTaskInLoop,this,id));
}

void TimeWheel::UnableTimeTask(size_t id)
{
    _loop->RunInLoop(std::bind(&TimeWheel::UnableTimeTaskInLoop,this,id));
}

void TimeWheel::AddTimeTask(size_t id,size_t timeout,TimeOutCallBack cb)
{
    _loop->RunInLoop(std::bind(&TimeWheel::AddTimeTaskInLoop,this,id,timeout,cb));
}

void TimeWheel::RefreshTimeTask(size_t id)
{
    _loop->RunInLoop(std::bind(&TimeWheel::RefreshTimeTaskInLoop,this,id));
}
void TimeWheel::DeleteTimeTask(size_t id)
{
    _loop->RunInLoop(std::bind(&TimeWheel::DeleteTimeTaskInLoop,this,id));
}