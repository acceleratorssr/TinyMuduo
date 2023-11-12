#ifndef _CHANNEL_HPP_
#define _CHANNEL_HPP_

#include "noncopyable.hpp"
#include "Timestamp.hpp"

#include <functional>
#include <memory>

class EventLoop;

class Channel : noncopyable
{
public:
    typedef std::function<void()> EventCallback;
    using ReadEventCallback = std::function<void(Timestamp)>;
    Channel(EventLoop *loop, int fd);
    ~Channel();

    void handleEvent(Timestamp receivetime);

    void setReadCallback(ReadEventCallback cb) { readcallback_ = std::move(cb); }
    void setWriteCallback(EventCallback cb) { writecallback_ = std::move(cb); }
    void setCloseCallback(EventCallback cb) { closecallback_ = std::move(cb); }
    void setErrorCallback(EventCallback cb) { errorcallback_ = std::move(cb); }

    void tie(const std::shared_ptr<void> &);

    int GetFd() { return fd_; }
    int GetRvents() { return events_; }
    void SetRevent(int revt) { revents_ = revt; }

    bool isNoneEvent() const { return kNoneEvent_ == events_; }
    bool isReadEvent() const { return kReadEvent_ & events_; }
    bool isWriteEvent() const { return kwriteEvent_ & events_; }

    void EnableReading()
    {
        events_ |= kReadEvent_;
        update();
    }
    void DisableReading()
    {
        events_ &= kReadEvent_;
        update();
    }
    void EnableWriting()
    {
        events_ |= kwriteEvent_;
        update();
    }
    void DisableWriting()
    {
        events_ &= kwriteEvent_;
        update();
    }
    void DisableAll()
    {
        events_ = kNoneEvent_;
        update();
    }

    int GetIndex() { return index_; }
    void SetIndex(int index) { index_ = index; }

    EventLoop *OwnerLoop() { return loop_; }
    void remove();

private:
    void update();
    void handleEventWithGuard(Timestamp receiveTime);

    static const int kNoneEvent_;
    static const int kReadEvent_;
    static const int kwriteEvent_;

    EventLoop *loop_;
    const int fd_;
    int events_;
    int revents_;
    int index_;

    std::weak_ptr<void> tie_;
    bool tied_;

    ReadEventCallback readcallback_;
    EventCallback writecallback_;
    EventCallback closecallback_;
    EventCallback errorcallback_;
};

#endif // _CHANNEL_HPP_
