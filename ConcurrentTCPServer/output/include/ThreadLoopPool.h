#pragma once

#include "lock.h"
#include "eventloop.h"
#include <thread>

class ThreadLoop;

class ThreadLoopPool
{
public:
    ThreadLoopPool(size_t thread_nums);
    EventLoop* SmartChooseThread();
    size_t _thread_nums;
    EventLoop * _baseloop;
    std::vector<ThreadLoop*> _threads;
    std::vector<EventLoop*> _loops;
};