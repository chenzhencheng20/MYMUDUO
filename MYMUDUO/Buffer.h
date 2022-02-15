#pragma once

#include<vector>
#include<string>

    /*
    *   我们在使用buffer时，其内部可能存有可读数据，这是给我们的服务器所读取的，readerIndex_记录的就是这些数据的起始位置。
    *   但是并没有所谓的可写数据（不要误以为有），有的只是buffer中剩余空间的大小，即可供我们服务器写数据的缓冲区大小，因此当buffer未满时，
    *   buffer_.size()-writeIndex_即剩余缓冲区的大小
    *   其实可以理解，writerIndex_即可读数据的尾端。 
    */

class Buffer
{
public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitiaSize = 1024;

    explicit Buffer(size_t initialSize = kInitiaSize)
        : buffer_(kCheapPrepend+kInitiaSize) //初始化vector容器的大小，这相当于resize
        , readerIndex_(kCheapPrepend)
        , writerIndex_(kCheapPrepend)
        {}

    //返回目前缓冲区中的可读字节数
    size_t readableBytes() const
    {
        return writerIndex_-readerIndex_;
    }

    //返回剩余可写空间的大小
    size_t writableBytes() const
    {
        return buffer_.size()-writerIndex_;
    }

    //返回可读数据位置的首地址 
    //注意：头部即8字节的位置与可读数据位置的首地址之间是可能有空间的（已经读了这部分数据就会留下空间）
    size_t prependableBytes() const
    {
        return readerIndex_;
    }

    //返回缓冲区中可写数据的起始地址
    const char* peek() const
    {
        return begin()+readerIndex_;
    }

    void retrieve(size_t len)
    {
        if(len<readableBytes())
        {
            readerIndex_+=len;//只读取了len长度，还有readerIndex_+len 至 writerIndex_的数据没读取
        }
        else //len==readableBytes 其实即使大于，可不会有什么影响
        {
            retrieveAll();
        }
    }

    void retrieveAll()
    {
        readerIndex_=writerIndex_=kCheapPrepend;
    }

    //把onMessage函数上报的Buffer数据转成string类型的数据并返回
    std::string retrieveAllAsString()
    {
        return retrieveAsString(readableBytes());
    }

    std::string retrieveAsString(size_t len)
    {
        std::string result(peek(),len);
        retrieve(len);//上一句已经把缓冲区里可读的数据读取了出来，现在要进行复位操作
        return result;
    }

    //要考虑可写缓冲区的大小够不够用，是否需要扩容
    void ensuerWriteableBytes(size_t len)
    {
        if(writableBytes()<len)
        {
            makeSpace(len); //扩容函数
        }
    }
    //把 [data,data+len]内存上的数据添加到writeable缓冲区中
    void append(const char*data,size_t len)
    {
        ensuerWriteableBytes(len);
        std::copy(data,data+len,beginWrite());
        writerIndex_+=len;
    }

    char* beginWrite()
    {
        return begin()+writerIndex_;
    }
    const char* beginWrite()const
    {
        return begin()+writerIndex_;
    }
    //从fd上读取数据
    ssize_t readFd(int fd,int *saveErrno);
    ssize_t writeFd(int fd,int*saveErrno);
    
private:
    //返回容器地址的 指针！
    char* begin()
    {
        return &*buffer_.begin();
    }

    const char* begin() const
    {
        return &*buffer_.begin();
    }

    //判断是需要扩容，还是需要空间整合
    void makeSpace(size_t len)
    {
        //很容易理解，左边perpendableBytes()减去kCheapPrepend其实就是头部与可读数据首元素之间的空隙，这里有时候是有空间的（数据已经被读掉）因此可以用
        if(writableBytes() + prependableBytes()<len + kCheapPrepend)
        {
            buffer_.resize(writerIndex_+len);//这样扩容，从writerIndex_开始正好可以容下len长度的数据
        }
        else
        {
            size_t readable = readableBytes();//获取剩余可读数据大小
            
            //将剩余可读数据提到前面，把空间整合
            std::copy(begin()+readableBytes(),
                        begin()+writerIndex_,
                        begin()+kCheapPrepend);
            //更新可读数据的起始位置
            readerIndex_=kCheapPrepend;
            //更新可写数据空间的起始位置
            writerIndex_=kCheapPrepend+readable;
        }
    }   

    std::vector<char> buffer_;
    size_t readerIndex_;//缓冲区中 存储所读数据的位置的起始位置
    size_t writerIndex_;//缓冲区中 可用于存储所写数据的位置的起始位置
};