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
    
/*---------------------------------------------------------
 * 请注意！
 * wakeupChannel_ 的初始化依赖于 wakeupFd_ 作为fd传入channel的ctor
 * 成员变量的初始化顺序取决于它们在类定义中的声明顺序，而非在成员初始化列表中的顺序
 * 若 wakeupFd_ 在 wakeupChannel_ 之前未被正确初始化，虽不会引起编译错误，但会导致程序运行时错误
 * 所以，应该需要先声明wakeupFd_，再声明wakeupChannel_
 * ---------------------------------------------------------
*/ 
    int wakeupFd_;
    std::unique_ptr<Channel> wakeupChannel_;
// -------------------

    ChannelList activeChannels_;

    std::atomic_bool callingPendingFunctors_;
    std::vector<Functor> pendingFunctors_;
    std::mutex mutex_;
};

#endif // _EVENTLOOP_HPP_
