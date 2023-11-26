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
    int64_t microSecondsSinceEpoch_;
};

#endif // _TIMESTAMP_HPP_
