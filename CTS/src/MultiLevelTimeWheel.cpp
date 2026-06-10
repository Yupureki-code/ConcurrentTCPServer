#include "../include/MultiLevelTimeWheel.h"
#include "../include/eventloop.h"

MultiLevelTimeWheel::MultiLevelTimeWheel(EventLoop* loop)
    : _second_wheel(60, nullptr)
    , _minute_wheel(60, nullptr)
    , _hour_wheel(24, nullptr)
    , _loop(loop)
{
}

MultiLevelTimeWheel::~MultiLevelTimeWheel() = default;

bool MultiLevelTimeWheel::AddTimeTask(size_t id, size_t timeout_sec, TimeOutCallBack cb)
{
    if (timeout_sec <= 60)
    {
        _task_level[id] = 0;
        _second_wheel.AddTimeTask(id, timeout_sec, cb);
    }
    else if (timeout_sec <= 3600)
    {
        _task_level[id] = 1;
        size_t minutes = (timeout_sec + 59) / 60;
        _minute_wheel.AddTimeTask(id, minutes, [this, id, timeout_sec, cb]() {
            size_t remaining = timeout_sec % 60;
            if (remaining > 0)
                _second_wheel.AddTimeTask(id, remaining, cb);
            else
                cb();
        });
    }
    else
    {
        _task_level[id] = 2;
        size_t hours = (timeout_sec + 3599) / 3600;
        _hour_wheel.AddTimeTask(id, hours, [this, id, timeout_sec, cb]() {
            size_t remaining = timeout_sec % 3600;
            if (remaining > 0)
            {
                size_t minutes = (remaining + 59) / 60;
                _minute_wheel.AddTimeTask(id, minutes, [this, id, remaining, cb]() {
                    size_t sec = remaining % 60;
                    if (sec > 0)
                        _second_wheel.AddTimeTask(id, sec, cb);
                    else
                        cb();
                });
            }
            else
                cb();
        });
    }
    return true;
}

void MultiLevelTimeWheel::DeleteTimeTask(size_t id)
{
    _second_wheel.DeleteTimeTask(id);
    _minute_wheel.DeleteTimeTask(id);
    _hour_wheel.DeleteTimeTask(id);
    _task_level.erase(id);
}

void MultiLevelTimeWheel::RefreshTimeTask(size_t id)
{
    auto it = _task_level.find(id);
    if (it == _task_level.end())
        return;
    
    switch (it->second)
    {
    case 0:
        _second_wheel.RefreshTimeTask(id);
        break;
    case 1:
        _minute_wheel.RefreshTimeTask(id);
        break;
    case 2:
        _hour_wheel.RefreshTimeTask(id);
        break;
    }
}

void MultiLevelTimeWheel::EnableTimeTask(size_t id)
{
    _second_wheel.EnableTimeTask(id);
    _minute_wheel.EnableTimeTask(id);
    _hour_wheel.EnableTimeTask(id);
}

void MultiLevelTimeWheel::UnableTimeTask(size_t id)
{
    _second_wheel.UnableTimeTask(id);
    _minute_wheel.UnableTimeTask(id);
    _hour_wheel.UnableTimeTask(id);
}

bool MultiLevelTimeWheel::IsTimeTaskExists(size_t id)
{
    return _second_wheel.IsTimeTaskExists(id) || 
           _minute_wheel.IsTimeTaskExists(id) || 
           _hour_wheel.IsTimeTaskExists(id);
}
