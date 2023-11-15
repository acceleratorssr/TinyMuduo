#ifndef _THREAD_HPP_
#define _THREAD_HPP_

#include "noncopyable.hpp"

#include <functional>
#include <string>
#include <thread>
#include <memory>
#include <atomic>
#include <mutex>
#include <condition_variable>

class Thread : noncopyable
{
public:
    using ThreadFunc = std::function<void()>;

    explicit Thread(ThreadFunc, const std::string &name = std::string());
    ~Thread();

    void start();
    void join();

    bool started() const { return started_; }
    pid_t GetTid() const { return tid_; }
    const std::string& name() const { return name_;}
    static int GetnumCreated() {return numCreated_;}

private:
    void setDefaultName();

    bool started_;
    bool joined_;
    std::shared_ptr<std::thread> thread_;
    pid_t tid_;
    ThreadFunc func_;
    std::string name_;
    static std::atomic_int numCreated_;
};

#endif // _THREAD_HPP_
