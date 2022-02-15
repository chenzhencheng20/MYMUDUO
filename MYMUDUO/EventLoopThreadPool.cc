#include"EventLoopThreadPool.h"
#include"EventLoopThread.h"
#include<memory>
#include<vector>

#include"Logger.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop*baseLoop,const std::string &nameArg)
    : baseLoop_(baseLoop)
    , name_(nameArg)
    , started_(false)
    , numThreads_(0)
    , next_(0)
{
    
}
EventLoopThreadPool::~EventLoopThreadPool()
{}

void EventLoopThreadPool::start(const ThreadInitCallback &cb)
{
    started_=true;

    for(int i=0;i<numThreads_;++i)
    {
        char buf[name_.size()+32]={0};
        snprintf(buf,sizeof buf,"%s%d",name_.c_str(),i);
        EventLoopThread *t = new EventLoopThread(cb,buf);
        threads_.push_back(std::unique_ptr<EventLoopThread>(t));

        loops_.push_back(t->startLoop());//底层创建一个线程，绑定一个新的EventLoop ，并将其返回
    }
    if(numThreads_==0&&cb)
    {
        //这里不是很理解
        //解释：因为只有mianloop了，所以回调就直接由mainloop来执行
        cb(baseLoop_);
    }
}

//如果工作在多线程中，baseLoop_以轮询的方式分配channel给subLoop
EventLoop* EventLoopThreadPool::getNextLoop()
{
    EventLoop *loop = baseLoop_;

    if (!loops_.empty()) // 通过轮询获取下一个处理事件的loop
    {
        loop = loops_[next_];
        ++next_;
        if (next_ >= loops_.size())
        {
            next_ = 0;
        }
    }

    return loop;
}

std::vector<EventLoop*> EventLoopThreadPool::getAllLoops()
{
    if(!loops_.empty())
    {
        return loops_;
    }
    else
    {
    return std::vector<EventLoop*>(1,baseLoop_);
    }
}
