#pragma once

#include "comm.hpp"
#include <memory>

class EventLoop;
class Poll;
/// Channel类用于封装文件描述符和相关事件的处理逻辑，提供了设置回调函数、启用/禁用事件等功能
class Channel
{
private:
    using EventCallBack = std::function<void()>;
public: 
    Channel(int fd,EventLoop* loop = nullptr);
    int GetFd();//获取文件描述符
    void SetFd(int fd);//设置文件描述符
    void SetLoop(EventLoop* loop);//设置事件循环
    uint32_t GetEvents();//获取感兴趣的事件
    void SetREvents(uint32_t events);//设置实际发生的事件
    void SetEventCallBack(const EventCallBack& cb);//设置事件回调函数
    void SetReadCallBack(const EventCallBack& cb);//设置读事件回调函数
    void SetWriteCallBack(const EventCallBack& cb);//设置写事件回调函数
    void SetErrorCallBack(const EventCallBack& cb);//设置错误事件回调函数
    void SetCloseCallBack(const EventCallBack& cb);//设置关闭事件回调函数
    void Tie(const std::shared_ptr<void>& ptr);//绑定一个shared_ptr，防止在事件处理过程中对象被销毁
    bool IsReadAble();//判断是否可读
    bool IsWriteAble();//判断是否可写
    void EnableRead();//启用读事件
    void EnableWrite();//启用写事件
    void UnableRead();//禁用读事件
    void UnableWrite();//禁用写事件
    void HandlerEvent();//处理事件
    void Add();//将Channel添加到事件循环中
    void Remove();//将Channel从事件循环中移除
    void HandlerEventWithGuard();//带保护的事件处理函数，确保在事件处理过程中对象不会被销毁
private:
    int _fd;//文件描述符
    uint32_t _events;//感兴趣的事件
    uint32_t _revents;//实际发生的事件
    EventLoop* _loop;//事件循环
    EventCallBack _event;//事件回调函数
    EventCallBack _read;//读事件回调函数
    EventCallBack _write;//写事件回调函数
    EventCallBack _error;//错误事件回调函数
    EventCallBack _close;//关闭事件回调函数
    std::weak_ptr<void> _tie;//绑定的shared_ptr，防止在事件处理过程中对象被销毁
    bool _is_tied = false;//是否绑定了shared_ptr
};