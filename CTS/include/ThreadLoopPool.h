#pragma once

#include <atomic>
#include <memory>
#include <vector>

class EventLoop;
class ThreadLoop;

class ThreadLoopPool
{
public:
	explicit ThreadLoopPool(size_t thread_nums);
	~ThreadLoopPool();
	void SetBaseLoop(EventLoop* loop);
	EventLoop* SmartChooseThread();

private:
	size_t _thread_nums;
	EventLoop* _baseloop;
	std::vector<std::unique_ptr<ThreadLoop>> _threads;
	std::vector<EventLoop*> _loops;
	std::atomic<size_t> _next_index;
};
