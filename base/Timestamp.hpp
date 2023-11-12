#ifndef _TIMESTAMP_HPP_
#define _TIMESTAMP_HPP_

#include "copyable.hpp"

#include <iostream>
#include <string>
#include <chrono>

class Timestamp : public copyable
{
public:
    Timestamp();
    explicit Timestamp(int64_t microSecondsSinceEpoch);
    static Timestamp now();
    std::string toString() const;
private:
    int64_t microSecondsSinceEpoch_;//新世纪以来的微秒 -> UNIX 时间戳
};

#endif  //_TIMESTAMP_HPP_
