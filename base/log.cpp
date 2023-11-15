#include <iostream>

#include "log.hpp"
#include "Timestamp.hpp"

Logger &Logger::GetInstance()
{
    static Logger logger;
    return logger;
}

void Logger::Log(std::string message)
{
    switch (loglevel_)
    {
    case DEBUG:
        std::cout << "[DEBUG]   ";
        break;
    case TRACE:
        std::cout << "[TRACE]   ";
        break;
    case INFO:
        std::cout << "[INFO]    ";
        break;
    case WARNNING:
        std::cout << "[WARNING] ";
        break;
    case ERROR:
        std::cout << "[ERROR]   ";
        break;
    case FATAL:
        std::cout << "[FATAL]   ";
        break;
    }
    std::cout << Timestamp::now().toString() << " : " << message << std::endl;
}

void Logger::SetLogLevel(int level)
{
    loglevel_ = level;
}
