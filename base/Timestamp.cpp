// #include "timestamp.hpp"
// #include "/home/acc/myMuduo/MyMuduo/timestamp.hpp"
#include "./Timestamp.hpp"
/*
第四种include方式，使用g++ -o t timestamp.cpp -std=c++11
报错：
timestamp.cpp:3:33: fatal error: MyMuduo/timestamp.hpp: No such file or directory
    #include "MyMuduo/timestamp.hpp"
                                    ^
compilation terminated.*/
// #include "MyMuduo/timestamp.hpp"


Timestamp::Timestamp() : microSecondsSinceEpoch_(0)
{
}

Timestamp::Timestamp(int64_t microSecondsSinceEpoch) : microSecondsSinceEpoch_(microSecondsSinceEpoch)
{
}

Timestamp Timestamp::now()
{
    auto currentTime = std::chrono::system_clock::now();
    std::time_t currentTime_t = std::chrono::system_clock::to_time_t(currentTime);
    return Timestamp(currentTime_t);
}

std::string Timestamp::toString() const
{
    char buf[128] = {0};
    std::tm *localTime = std::localtime(&microSecondsSinceEpoch_);
    int year = localTime->tm_year + 1900; // tm_year表示的是从1900年开始的年份
    int month = localTime->tm_mon + 1;    // tm_mon表示的月份是0到11
    int day = localTime->tm_mday;
    int hour = localTime->tm_hour;
    int minute = localTime->tm_min;
    int second = localTime->tm_sec;
    snprintf(buf, 128, "%4d.%02d.%02d %02d:%02d:%02d",
             year, month, day, hour, minute, second);
    return buf;
}
