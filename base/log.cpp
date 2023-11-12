/*****************************************************/
/*
start:  2023/10/28
end:    2023/10/28

Description:
日志代码，利用宏和可变参数方便输出

Point:
1.可变参数的使用允许在宏中接受任意数量的参数，然后将这些参数传递给宏的实际操作。
在 LOG_DEBUG 中，... 的作用是将参数传递给 snprintf 函数，以便将参数填充到 logmsgformat 中，生成格式化的日志消息。

2.## 运算符只在参数列表中有可变数量的参数时使用，以避免在没有参数时产生额外的逗号。
如果知道宏始终会有参数，可以不使用 ##。

3.GetInstance()是线程安全的，
SetLogLevel()和Log()线程不安全

Author：acc
*/
/*****************************************************/

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
