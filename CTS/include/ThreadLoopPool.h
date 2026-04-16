#pragma once

#include <atomic>
#include <memory>
#include <vector>

class EventLoop;
class ThreadLoop;
/// ThreadLoopPool类用于管理多个线程和对应的事件循环，提供了智能选择线程的功能，以实现负载均衡
class ThreadLoopPool
{
public:
	explicit ThreadLoopPool(size_t thread_nums);//构造函数，初始化线程池，创建指定数量的线程和事件循环
	~ThreadLoopPool();
	void SetBaseLoop(EventLoop* loop);//设置基础事件循环，通常是主线程的事件循环
	EventLoop* SmartChooseThread();//智能选择一个线程的事件循环，通常采用轮询的方式，以实现负载均衡

private:
	size_t _thread_nums;//线程数量
	EventLoop* _baseloop;//基础事件循环，通常是主线程的事件循环
	std::vector<std::unique_ptr<ThreadLoop>> _threads;//线程对象的智能指针数组，管理线程的生命周期
	std::vector<EventLoop*> _loops;//线程对应的事件循环指针数组，方便根据线程索引获取对应的事件循环
	std::atomic<size_t> _next_index;//下一个线程索引，使用原子类型以保证线程安全
};
