#pragma once
#include<arpa/inet.h>
#include<netinet/in.h>
#include<string>

//设置 sockaddr_in(内含IP，端口等)
class InetAddress
{
public:
    
    explicit InetAddress(uint16_t port=0,std::string ip = "127.0.0.1");
    explicit InetAddress(const sockaddr_in &addr):addr_(addr){};
    //得到IP
    std::string toIp() const;
    //IP+端口
    std::string toIpPort() const;
    uint16_t toPort() const;
    //直接得到sockaddr_in
    const sockaddr_in*getSockAddr () const {return &addr_;}
    
    void setSockAddr(const sockaddr_in addr){ addr_=addr; }
private:
    sockaddr_in addr_;
};