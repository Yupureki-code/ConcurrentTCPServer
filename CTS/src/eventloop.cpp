#include "../include/eventloop.h"
#include "../include/channel.h"
#include "../include/logstrategy.hpp"

bool EventLoop::IsInLoop(){ return _thread_id == std::this_thread::get_id(); }
void EventLoop::EnableTimeTask(size_t id){ _timewheel.EnableTimeTask(id); }
void EventLoop::UnableTimeTask(size_t id){ _timewheel.UnableTimeTask(id); }
void EventLoop::AddTimeTask(size_t id,size_t timeout,TimeOutCallBack cb){ _timewheel.AddTimeTask(id, timeout, cb); }
void EventLoop::RefreshTimeTask(size_t id){ _timewheel.RefreshTimeTask(id); }
void EventLoop::DeleteTimeTask(size_t id){ _timewheel.DeleteTimeTask(id); }
void EventLoop::AddChannel(Channel* channel){ _poll.AddChannel(channel); }
void EventLoop::RemoveChannel(Channel* channel){ _poll.RemoveChannel(channel); }
int EventLoop::GetConsNums(){ return _poll.GetPollNums(); }

EventLoop::EventLoop()
    :_thread_id(std::this_thread::get_id()),_timewheel(60,this),_poll(0)
{
    _event_fd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
    if(_event_fd < 0)
    {
        logger(ns_log::FATAL)<<"eventfd create error!";
        exit(ExitCode::EVENTFD_CREATE);
    }
    logger(ns_log::INFO) << "EventLoop event_fd=" << _event_fd;
    _event_channel = std::make_unique<Channel>(_event_fd, this);
    _event_channel->SetReadCallBack(std::bind(&EventLoop::ReadEvent,this));
    logger(ns_log::INFO) << "EventLoop::EventLoop() poll epfd=" << _poll.GetEpfd();
    _event_channel->EnableRead();
    logger(ns_log::INFO) << "EventLoop::EventLoop() done";
}

void EventLoop::ReadEvent()
{
    uint64_t ret;
    int n = read(_event_fd,&ret,sizeof(ret));
    if(n < 0)
    {
        logger(ns_log::FATAL)<<"eventfd read error!";
        exit(EVENTFD_READ);
    }
}

void EventLoop::WakeUpEvent()
{
    uint64_t ret = 1;
    int n = write(_event_fd,&ret,sizeof(ret));
    if(n < 0)
    {
        logger(ns_log::FATAL)<<"eventfd write error!";
        exit(EVENTFD_WRITE);
    }
}

void EventLoop::Run()
{
    while(1)
    {
        std::vector<Channel*> active;
        _poll.Run(active);
        for(auto & it : active)
            (*it).HandlerEvent();
        RunAllTasks();
    }
}

void EventLoop::RunAllTasks()
{
    std::queue<func> tasks;
    _mutex.lock();
    std::swap(tasks, _task_queue);
    _mutex.unlock();

    while(tasks.size())
    {
        tasks.front()();
        tasks.pop();
    }
}

void EventLoop::RunInLoop(const func& cb)
{
    if(IsInLoop())
        cb();
    else
        PushInLoop(cb);
}

void EventLoop::PushInLoop(const func& cb)
{
    _mutex.lock();
    _task_queue.push(cb);
    _mutex.unlock();
    WakeUpEvent();
}