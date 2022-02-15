#pragma once

#include<string>

#include"noncopyable.h"


//宏为多行时，每行后面先空格再加\再回车
//LOG_INFO("%s %d",arg1,arg2)
#define LOG_INFO(logmsgFormat, ...) \
    do \
    { \
        Logger &logger = Logger::instance(); \
        logger.setLogLevel(INFO); \
        char buf[1024]={0}; \
        snprintf(buf,1024,logmsgFormat,##__VA_ARGS__); \
        logger.log(buf); \
    } while(0) 
#define LOG_ERROR(logmsgFormat, ...) \
    do \
    { \
        Logger &logger = Logger::instance(); \
        logger.setLogLevel(ERROR); \
        char buf[1024]={0}; \
        snprintf(buf,1024,logmsgFormat,##__VA_ARGS__); \
        logger.log(buf); \
    } while(0) 
#define LOG_FATAL(logmsgFormat, ...) \
    do \
    { \
        Logger &logger = Logger::instance(); \
        logger.setLogLevel(FATAL); \
        char buf[1024]={0}; \
        snprintf(buf,1024,logmsgFormat,##__VA_ARGS__); \
        logger.log(buf); \
        exit(-1); \
    } while(0) 
//程序正常运行时，没必要打印DEBUG信息，按以下设置，则只有定义MUDEBUG时，才输出DEBUG
#ifdef MUDEBUG
#define LOG_DEBUG(logmsgFormat, ...) \
    do \
    { \
        Logger &logger = Logger::instance(); \
        logger.setLogLevel(DEBUG); \
        char buf[1024]={0}; \
        snprintf(buf,1024,logmsgFormat,##__VA_ARGS__); \
        logger.log(buf); \
    } while(0) 
#else 
    #define LOG_DEBUG(logmsgFormat, ...)
#endif


//日志级别有：INFO, ERROR, FATAL, DEBUG
enum LogLevel
{
    INFO,   //普通信息
    ERROR,  //错误信息
    FATAL, //core信息（程序无可挽回，必须关闭时）
    DEBUG,   //调试信息
};

//输出一个日志类
class Logger:noncopyable
{
public:
    //获取日志唯一实例对象
    static Logger& instance();
    //设置日志级别
    void setLogLevel(int level);
    //写日志
    void log(std::string msg);

private:
    int logLevel_;
    Logger(){};

};