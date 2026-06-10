#pragma once

#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <atomic>

class WorkerThreadPool
{
public:
    explicit WorkerThreadPool(size_t thread_nums);
    ~WorkerThreadPool();
    
    void Submit(std::function<void()> task);
    void Shutdown();
    size_t GetThreadNums() const;
    
private:
    void WorkerEntry();
    
    std::vector<std::thread> _threads;
    std::queue<std::function<void()>> _tasks;
    std::mutex _mutex;
    std::condition_variable _cv;
    std::atomic<bool> _running{true};
    size_t _thread_nums;
};
