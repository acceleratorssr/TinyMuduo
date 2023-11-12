/*****************************************************/
/*
start:  2023/10/28
end:    2023/10/28

Description:
日志代码，单例模式

Point:
单例模式，记得构造函数私有化（防止外部实例化），可将实例的访问点封装在一个公共静态方法中，
类的使用者只需要调用该方法来获取实例

Author：acc
*/
/*****************************************************/

#ifndef _LOG_HPP_
#define _LOG_HPP_

#include "noncopyable.hpp"

#include <string>

#ifdef DEBUG
#define LOG_DEBUG(logmsgformat, ...)                      \
    do                                                    \
    {                                                     \
        Logger &logger = Logger::GetInstance();           \
        logger.SetLogLevel(DEBUG);                        \
        char buf[1024] = {0};                             \
        snprintf(buf, 1024, logmsgformat, ##__VA_ARGS__); \
        ;                                                 \
        logger.Log(buf);                                  \
    } while (0)

#else
#define LOG_DEBUG(logmsgformat, ...)
#endif

#define LOG_TRACE(logmsgformat, ...)                       \
    do                                                    \
    {                                                     \
        Logger &logger = Logger::GetInstance();           \
        logger.SetLogLevel(TRACE);                         \
        char buf[1024] = {0};                             \
        snprintf(buf, 1024, logmsgformat, ##__VA_ARGS__); \
        logger.Log(buf);                                  \
    } while (0)

#define LOG_INFO(logmsgformat, ...)                       \
    do                                                    \
    {                                                     \
        Logger &logger = Logger::GetInstance();           \
        logger.SetLogLevel(INFO);                         \
        char buf[1024] = {0};                             \
        snprintf(buf, 1024, logmsgformat, ##__VA_ARGS__); \
        logger.Log(buf);                                  \
    } while (0)

#define LOG_WARNNING(logmsgformat, ...)                   \
    do                                                    \
    {                                                     \
        Logger &logger = Logger::GetInstance();           \
        logger.SetLogLevel(WARNNING);                     \
        char buf[1024] = {0};                             \
        snprintf(buf, 1024, logmsgformat, ##__VA_ARGS__); \
        logger.Log(buf);                                  \
    } while (0)

#define LOG_ERROR(logmsgformat, ...)                      \
    do                                                    \
    {                                                     \
        Logger &logger = Logger::GetInstance();           \
        logger.SetLogLevel(ERROR);                        \
        char buf[1024] = {0};                             \
        snprintf(buf, 1024, logmsgformat, ##__VA_ARGS__); \
        logger.Log(buf);                                  \
    } while (0)

#define LOG_FATAL(logmsgformat, ...)                      \
    do                                                    \
    {                                                     \
        Logger &logger = Logger::GetInstance();           \
        logger.SetLogLevel(FATAL);                        \
        char buf[1024] = {0};                             \
        snprintf(buf, 1024, logmsgformat, ##__VA_ARGS__); \
        logger.Log(buf);                                  \
        exit(-1);                                         \
    } while (0)

enum
{
    DEBUG,
    TRACE,
    INFO,
    WARNNING,
    ERROR,
    FATAL
};

class Logger : noncopyable // 单例模式
{
public:
    static Logger &GetInstance();
    void Log(std::string message);
    void SetLogLevel(int level);

private:
    Logger() {}
    int loglevel_;
};

#endif //_LOG_HPP_