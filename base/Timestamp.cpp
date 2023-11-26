#include "./Timestamp.hpp"

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
    int year = localTime->tm_year + 1900;
    int month = localTime->tm_mon + 1;
    int day = localTime->tm_mday;
    int hour = localTime->tm_hour;
    int minute = localTime->tm_min;
    int second = localTime->tm_sec;
    snprintf(buf, 128, "%4d.%02d.%02d %02d:%02d:%02d",
             year, month, day, hour, minute, second);
    return buf;
}
