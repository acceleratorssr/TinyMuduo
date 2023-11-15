#ifndef _EVENTLOOP_HPP_
#define _EVENTLOOP_HPP_

#include <vector>
#include <atomic>
#include <memory>
#include <mutex>

#include "noncopyable.hpp"
#include "Timestamp.hpp"
#include "currentThread.hpp"

class Channel;
class Poller;

class EventLoop : noncopyable
{
public:
    EventLoop();
    ~EventLoop();
    using Functor = std::function<void()>;

    void loop();
    void quit();

    Timestamp pollReturnTime() const { return pollReturnTime_; }

    void runInLoop(Functor cb);
    void QueueInLoop(Functor cb);

    void wakeup();

    void updateChannel(Channel *channel);
    void removeChannel(Channel *channel);
    bool hasChannel(Channel *channel);

    bool isInLoopThread() const { return threadId_ == CurrentThread::GetTid(); }

private:
    void handleRead();
    void doPendingFunctors();

    using ChannelList = std::vector<Channel *>;

    std::atomic_bool looping_;
    std::atomic_bool quit_;

    const pid_t threadId_;

    Timestamp pollReturnTime_;
    std::unique_ptr<Poller> poller_;

    std::unique_ptr<Channel> wakeupChannel_;
    ChannelList activeChannels_;

    int wakeupFd_;

    std::atomic_bool callingPendingFunctors_;
    std::vector<Functor> pendingFunctors_;
    std::mutex mutex_;
};

#endif // _EVENTLOOP_HPP_
