#pragma once

#include "comm.hpp"
#include "channel.h"
#include <memory>
#include <queue>
#include <sys/timerfd.h>

class EventLoop;

#define DEFAULT_TIMEWHEEL_NUM 60

using TimeOutCallBack = std::function<void()>;
using ReleaseCallBack = std::function<void()>;
/// TimeTask类用于表示一个定时任务，包含任务ID、超时时间、是否启用以及超时回调和释放回调函数
class TimeTask
{
public:
    TimeTask(size_t id,int timeout,TimeOutCallBack timeout_cb,ReleaseCallBack release_cb)
    :_id(id),_timeout(timeout),_timeout_cb(timeout_cb),_release_cb(release_cb)
    {}
    size_t delay_time(){ return _timeout;}
    ~TimeTask()//析构函数，执行超时回调和释放回调函数
    {
        if(_timeout_cb && _is_enable)
            _timeout_cb();
        if(_release_cb)
            _release_cb();
    }
    void Enable(){_is_enable = true;}//启用定时任务
    void Unable(){_is_enable = false;}//禁用定时任务

private:
    size_t _id;//定时任务ID
    size_t _timeout;//定时任务的超时时间，单位为秒
    bool _is_enable = true;//定时任务是否启用，默认为启用状态
    TimeOutCallBack _timeout_cb;//定时任务超时回调函数，在定时任务超时时被调用
    ReleaseCallBack _release_cb;//定时任务释放回调函数，在定时任务被销毁时被调用
};
/// TimeWheel类用于管理定时任务，提供了添加、删除、启用、禁用定时任务以及运行定时任务等功能
class TimeWheel
{
private:
    using SharedTimeTask = std::shared_ptr<TimeTask>;//定时任务的智能指针类型
    using WeakTimeTask = std::weak_ptr<TimeTask>;//定时任务的弱指针类型
    static void RemoveWeakTask(TimeWheel* ptr,size_t id){ptr->RemoveWeakTaskHelper(id);}//静态函数，用于从弱指针中移除定时任务，防止定时任务被销毁后仍然存在于时间轮中
    void RemoveWeakTaskHelper(size_t id);//从弱指针中移除定时任务的辅助函数，实际执行移除操作
    void EnableTimeTaskInLoop(size_t id);//在事件循环中启用定时任务
    void UnableTimeTaskInLoop(size_t id);//在事件循环中禁用定时任务
    bool AddTimeTaskInLoop(size_t id,size_t timeout,TimeOutCallBack cb);//在事件循环中添加定时任务
    void RefreshTimeTaskInLoop(size_t id);//在事件循环中刷新定时任务，即重新计算定时任务的超时时间并将其放入正确的位置
    void DeleteTimeTaskInLoop(size_t id);//在事件循环中删除定时任务
public:
    TimeWheel(int capacity = DEFAULT_TIMEWHEEL_NUM,EventLoop* loop = nullptr);//构造函数，初始化时间轮的容量和事件循环
    bool IsTimeTaskExists(size_t id);//判断定时任务是否存在
    void RunOneTime();//运行一次时间轮，检查当前时间轮槽位中的定时任务是否超时，并执行相应的回调函数
    int CreateTimeFd();//创建一个timerfd，用于在定时任务超时时触发事件
    int ReadTimeFd();//读取timerfd的事件，获取超时的定时任务数量
    size_t GetTick() const;//获取当前时间轮的刻度，即当前时间轮槽位的索引
    void EnableTimeTask(size_t id);//启用定时任务
    void UnableTimeTask(size_t id);//禁用定时任务
    void AddTimeTask(size_t id,size_t timeout,TimeOutCallBack cb);//添加定时任务，指定定时任务ID、超时时间和超时回调函数
    void RefreshTimeTask(size_t id);//刷新定时任务，即重新计算定时任务的超时时间并将其放入正确的位置
    void DeleteTimeTask(size_t id);//删除定时任务，指定定时任务ID
private:
    std::vector<std::queue<SharedTimeTask>> _timewheel;//时间轮，使用一个向量来表示时间轮的槽位，每个槽位是一个队列，用于存储在该槽位上超时的定时任务
    std::unordered_map<size_t,WeakTimeTask> _timers;//定时任务的映射关系，使用一个哈希表来存储定时任务ID和对应的弱指针，方便根据定时任务ID找到对应的定时任务
    size_t _capacity;//时间轮的容量，即槽位的数量
    int _tick = 0;//当前时间轮的刻度，即当前时间轮槽位的索引
    int _timefd;//用于触发定时任务超时事件的timerfd文件描述符
    std::unique_ptr<Channel> _time_channel;//用于监听timerfd事件的Channel对象
    EventLoop* _loop;//事件循环，用于在事件循环中执行定时任务的相关操作
};