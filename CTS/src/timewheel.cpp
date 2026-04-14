#include "../include/timewheel.h"
#include "../include/eventloop.h"

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

bool TimeWheel::AddTimeTaskInLoop(size_t id,size_t timeout,TimeOutCallBack cb)
{
    if(IsTimeTaskExists(id))
    {
        RefreshTimeTaskInLoop(id);
        return true;
    }
    if(timeout > _capacity)
        return false;
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
    _tick = (_tick + 1)%_capacity;
    auto& q = _timewheel[_tick];
    while(!q.empty())
    {
        auto task = std::move(q.front());
        q.pop();
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