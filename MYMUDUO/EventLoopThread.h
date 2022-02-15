#pragma once

#include"noncopyable.h"
#include"Thread.h"

#include<functional>
#include<mutex>
#include<condition_variable>//条件变量头文件
#include<string>

class EventLoop;

// one loop per thread
class EventLoopThread:noncopyable
{
public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;

    EventLoopThread(const ThreadInitCallback&cb = ThreadInitCallback(),
    const std::string &name=std::string() );
    ~EventLoopThread();

    EventLoop*startLoop();//开启循环

private:
    //线程函数，在里面创建loop
    void threadFunc();

    EventLoop*loop_;
    bool exiting_; //是否退出循环
    Thread thread_;
    std::mutex mutex_;//互斥锁
    std::condition_variable cond_;//条件变量
    ThreadInitCallback callback_;
};