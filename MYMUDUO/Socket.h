#pragma once

#include"noncopyable.h"
#include"InetAddress.h"

class Socket:noncopyable
{
public:
    explicit Socket(int sockfd)
    : sockfd_(sockfd) {}
    
    ~Socket();

    int fd()const { return sockfd_; }
    void bindAddress(const InetAddress &localaddr);
    void listen();
    int accept(InetAddress *peeraddr);

    void shutdownWrite();

    //直接发送，对数据不进行tcp缓冲
    void setTcpNoDelay(bool on);
    //以下都是一些编译选项
    void setReuseAddr(bool on);
    void setReusePort(bool on);
    void setKeepAlive(bool on);
private:
    const int sockfd_;
};