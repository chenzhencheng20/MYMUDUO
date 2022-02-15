#include"Thread.h"

#include"CurrentThread.h"
#include<semaphore.h>

std::atomic_int Thread::numCreated_ = {0};

Thread::Thread(ThreadFunc func,const std::string &name)
    :started_(false)
    ,joined_(false)
    ,tid_(0)
    ,func_(std::move(func))
    ,name_(name)
{
    setDefaultName();
}

Thread::~Thread()
{
    //线程已经开启，并且不在工作，才可以析构它
    if(started_ && !joined_)
    {
        thread_->detach();//thread类提供了设置分类线程的方法
    }
}

//一个Thread对象，记录的是一个新线程的详细信息
void Thread::start()
{
    started_=true;

    /*
    *   这里用到信号量的考量是，必须等待线程创建以后，才能跳出start函数，因此用到信号量
    */
    //信号量
    sem_t sem;
    sem_init(&sem,false,0);

    //开启线程
    thread_=std::shared_ptr<std::thread>(new std::thread([&](){
        //获取线程tid值
        tid_=CurrentThread::tid();

        sem_post(&sem);

        //开启一个新线程，专门执行该函数
        func_();

    }));

    //这里必须等待获取上面新创建的线程的tid值
    sem_wait(&sem);
}

void Thread::join()
{
    joined_=true;
    thread_->join();
}

void Thread::setDefaultName()
{
    int num=++numCreated_;
    if(name_.empty())
    {
        char buf[32]={0};
        snprintf(buf,sizeof buf,"Thread %d",num);
        name_=buf;
    }

}