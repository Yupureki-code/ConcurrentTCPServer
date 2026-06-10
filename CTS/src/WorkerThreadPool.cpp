#include "../include/WorkerThreadPool.h"

WorkerThreadPool::WorkerThreadPool(size_t thread_nums)
    : _thread_nums(thread_nums)
{
    for (size_t i = 0; i < _thread_nums; ++i)
    {
        _threads.emplace_back(&WorkerThreadPool::WorkerEntry, this);
    }
}

WorkerThreadPool::~WorkerThreadPool()
{
    Shutdown();
}

void WorkerThreadPool::WorkerEntry()
{
    while (_running.load())
    {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _cv.wait(lock, [this]() {
                return !_tasks.empty() || !_running.load();
            });
            
            if (!_running.load() && _tasks.empty())
                return;
            
            task = std::move(_tasks.front());
            _tasks.pop();
        }
        task();
    }
}

void WorkerThreadPool::Submit(std::function<void()> task)
{
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _tasks.push(std::move(task));
    }
    _cv.notify_one();
}

void WorkerThreadPool::Shutdown()
{
    _running.store(false);
    _cv.notify_all();
    for (auto& thread : _threads)
    {
        if (thread.joinable())
            thread.join();
    }
    _threads.clear();
}

size_t WorkerThreadPool::GetThreadNums() const
{
    return _thread_nums;
}
