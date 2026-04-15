#include "../include/ThreadLoopPool.h"

#include "../include/cond.h"
#include "../include/eventloop.h"
#include "../include/lock.h"
#include "../include/logstrategy.hpp"
#include <cassert>
#include <string>
#include <thread>

class ThreadLoop
{
private:
    void ThreadEntry()
    {
        EventLoop loop;
        _loop = &loop;
        _cond.broadcast();
        loop.Run();
    }

public:
    ThreadLoop()
        : _thread(&ThreadLoop::ThreadEntry, this)
    {}

    ~ThreadLoop()
    {
        if (_thread.joinable())
            _thread.detach();
    }

    EventLoop* GetLoop()
    {
        _mutex.lock();
        while (_loop == nullptr)
            _cond.wait(_mutex.get_lock());
        _cond.broadcast();
        _mutex.unlock();
        return _loop;
    }

private:
    mycond _cond;
    mylock _mutex;
    EventLoop* _loop = nullptr;
    std::thread _thread;
};

ThreadLoopPool::ThreadLoopPool(size_t thread_nums)
    : _thread_nums(thread_nums)
    , _baseloop(nullptr)
    , _threads(thread_nums)
    , _loops(thread_nums)
    , _next_index(0)
{
    for (size_t i = 0; i < _thread_nums; ++i)
    {
        _threads[i] = std::make_unique<ThreadLoop>();
        _loops[i] = _threads[i]->GetLoop();
    }
}

ThreadLoopPool::~ThreadLoopPool() = default;

void ThreadLoopPool::SetBaseLoop(EventLoop* loop)
{
    _baseloop = loop;
}

EventLoop* ThreadLoopPool::SmartChooseThread()
{
    logger(ns_log::DEBUG) << "进入SmartChooseThread";
    if (_thread_nums == 0)
    {
        assert(_baseloop != nullptr);
        return _baseloop;
    }
    size_t pos = _next_index.fetch_add(1, std::memory_order_relaxed) % _thread_nums;
    logger(ns_log::DEBUG) << "选择loop-id:" << std::to_string(pos);
    return _loops[pos];
}