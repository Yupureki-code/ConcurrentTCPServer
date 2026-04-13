#pragma once

#include "comm.hpp"
#include "lock.hpp"
#include "cond.hpp"
#include "eventloop.h"
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
    :_thread_nums(thread_nums),_threads(thread_nums),_loops(thread_nums)
    {
        for(int i = 0;i<_thread_nums;i++)
        {
            _threads[i] = new ThreadLoop;
            _loops[i] = _threads[i]->GetLoop();
        }
    }
    EventLoop* SmartChooseThread()
    {
        if(_thread_nums == 0)
            return _baseloop;
        int ret = 1e8;int pos = -1;
        for(int i = 0;i<_thread_nums;i++)
        {
            if(_loops[i]->GetConsNums() < ret)
            {
                ret = _loops[i]->GetConsNums();
                pos = i;
            }
        }
        return _loops[pos];
    }
    size_t _thread_nums;
    EventLoop * _baseloop;
    std::vector<ThreadLoop*> _threads;
    std::vector<EventLoop*> _loops;
};