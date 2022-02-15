#include"Acceptor.h"
#include"Logger.h"
#include"InetAddress.h"

#include<unistd.h>
#include<errno.h>
#include<sys/socket.h>
#include<sys/types.h>



static int createNonblocking()
{   //IPPROTO_TCP 改成0
    int sockfd = socket(AF_INET,SOCK_CLOEXEC|SOCK_NONBLOCK|SOCK_STREAM,0);
    if(sockfd<0)
    {
        LOG_FATAL("%s:%s:%d listen sockfd creat error:%d\n",__FILE__,__func__,__LINE__,errno);
    }
    //个人认为需要加上这句
    return sockfd;
}


Acceptor::Acceptor(EventLoop*loop,const InetAddress &listenAddr,bool reuseport)
    : loop_(loop)
    , acceptSocket_(createNonblocking())
    , acceptChannel_(loop,acceptSocket_.fd())
    , listenning_(false)
{
    acceptSocket_.setReuseAddr(true);
    acceptSocket_.setReusePort(true);
    acceptSocket_.bindAddress(listenAddr);
    //TcpServer::start() Acceptor.listen 有新用户连接，要执行一个回调
    //baseloop => accpetChannel_(listen)
    acceptChannel_.setReadCallback(std::bind(&Acceptor::handleRead,this));
}
Acceptor::~Acceptor()
{
    acceptChannel_.disableAll();
    acceptChannel_.remove();
}


void Acceptor::listen()
{
    listenning_=true;
    acceptSocket_.listen();
    acceptChannel_.enableReading();
}


void Acceptor::handleRead()
{
    InetAddress peerAddr;
    LOG_INFO(" jin ru acceptor:: handleRead!!!!!!!!!!!!!\n");
    int connfd = acceptSocket_.accept(&peerAddr);
    if(connfd>=0)
    {
        if(newConnectionCallback_)
        {
            LOG_INFO("in acceptir : is going to using newconnectioncallback!!!!!!!!!\n");
            newConnectionCallback_(connfd,peerAddr);//轮询找到subloop，唤醒，分发当前的新客户端的Channel
        }
        else
        {
            ::close(connfd);
        }
    }
    else
    {
        LOG_ERROR("%s:%s:%d listen sockfd creat error:%d\n",__FILE__,__func__,__LINE__,errno);
        if(errno==EMFILE)
        {
            LOG_ERROR("%s:%s:%d  sockfd reached limit:%d\n",__FILE__,__func__,__LINE__,errno);
        }
    }

}