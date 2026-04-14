#pragma once

#include <cassert>
#include "lock.hpp"
#include "cond.hpp"
#include "eventloop.h"
#include "logstrategy.hpp"
#include <atomic>
#include <memory>
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
    :_thread(&ThreadLoop::ThreadEntry,this)
    {}
    ~ThreadLoop()
    {
        if(_thread.joinable())
            _thread.detach();
    }
    EventLoop* GetLoop()
    {
        _mutex.lock();
        while(_loop == nullptr)
            _cond.wait(_mutex.get_lock());
        _cond.broadcast();
        _mutex.unlock();
        return _loop;
    }

    int GetConsNums()
    {
        return _loop->GetConsNums();
    }
private:
    mycond _cond;
    mylock _mutex;
    EventLoop* _loop = nullptr;
    std::thread _thread;
};

class ThreadLoopPool
{
public:
    ThreadLoopPool(size_t thread_nums)
    :_thread_nums(thread_nums),_threads(thread_nums),_loops(thread_nums),_next_index(0)
    {
        for(int i = 0;i<_thread_nums;i++)
        {
            _threads[i] = std::make_unique<ThreadLoop>();
            _loops[i] = _threads[i]->GetLoop();
        }
    }
    void SetBaseLoop(EventLoop* _loop)
    {
        _baseloop = _loop;
    }
    EventLoop* SmartChooseThread()
    {
        logger(ns_log::DEBUG)<<"进入SmartChooseThread";
        if(_thread_nums == 0)
        {
            assert(_baseloop != nullptr);
            return _baseloop;
        }
        size_t pos = _next_index.fetch_add(1, std::memory_order_relaxed) % _thread_nums;
        logger(ns_log::DEBUG)<<"选择loop-id:"<<std::to_string(pos);
        return _loops[pos];
    }
    size_t _thread_nums;
    EventLoop * _baseloop;
    std::vector<std::unique_ptr<ThreadLoop>> _threads;
    std::vector<EventLoop*> _loops;
    std::atomic<size_t> _next_index;
};