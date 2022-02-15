#include"EventLoopThread.h"
#include"EventLoop.h"
#include"Logger.h"

EventLoopThread::EventLoopThread(const ThreadInitCallback&cb,const std::string &name)
    : loop_(nullptr)
    , exiting_(false)
    , thread_(std::bind(&EventLoopThread::threadFunc,this),name)
    , mutex_()
    , cond_()
    , callback_(cb)
{

}

EventLoopThread::~EventLoopThread()
{
    //意思是线程要退出了，也要将其事件循环loop退出
    exiting_=true;
    if(loop_!=nullptr)
    {
        loop_->quit();
        thread_.join();
    }

}

EventLoop* EventLoopThread::startLoop()
{
    thread_.start(); //启动底层的新线程

    EventLoop *loop = nullptr;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while(loop_==nullptr)//修改了此处，从loop变成loop_
        {
            cond_.wait(lock);
        }
        loop = loop_;
    }
    return loop;
}

//下面这个方法，是在单独的新线程里运行的
void EventLoopThread::threadFunc()
{
    EventLoop loop;//创建一个独立的eventloop,和上面的线程是一一对应的，one loop per thread

    if(callback_)
    {
        callback_(&loop);
    }

    {
        std::unique_lock<std::mutex>lock(mutex_);
        loop_=&loop;
        cond_.notify_one();
    }

    loop_->loop();//EventLoop loop =>Poller.poll

    std::unique_lock<std::mutex> lock(mutex_);
    loop_=nullptr;
}

//对startLoop函数和threadFunc函数之间关系的理解
/*
*   在构造EventLoopThread时（详见其构造函数），已经将threadFunc函数绑定给Thread下面的func函数；
*   当调用startLoop函数时，里面的 thread.start() 将创建一个线程，同时调用其func函数(已绑定了threadFunc)（详见thread下的start函数)
*   threadFunc将正式生成一个EvenLoop,
*   startLoop中生成的EventLoop* loop指针暂时设置为空，
*   （其实该函数最后要返回真正工作的EventLoop，即threadFunc函数中生成的EventLoop.沟通方法即EventLoopThread下面的loop_)
*   因此用上条件变量，将startLoop中的loop锁起来，等待threadFunc函数中，loop_ = &loop,然后再唤醒startLoop中的条件变量
*   这样将startLoop中的loop指针就可以指向 loop_了(loop=loop_),然后再return  loop;
*/