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

class Logger : noncopyable
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
