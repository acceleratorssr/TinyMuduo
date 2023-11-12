#include "Channel.hpp"
#include "log.hpp"

#include <sys/epoll.h>

class EventLoop;

Channel::Channel(EventLoop *loop, int fd)
    : loop_(loop),
      fd_(fd),
      events_(0),
      revents_(0),
      index_(-1),
      tied_(false)
{
    LOG_INFO("----------Channel up------------");
}

Channel::~Channel()
{
    LOG_INFO("----------Channel down----------");
}

const int Channel::kNoneEvent_ = 0;
const int Channel::kReadEvent_ = EPOLLIN | EPOLLPRI;
const int Channel::kwriteEvent_ = EPOLLOUT;

void Channel::handleEvent(Timestamp receivetime)
{
    std::shared_ptr<void> guard;
    if (tied_)
    {
        guard = tie_.lock();
        if (guard)
            handleEventWithGuard(receivetime);
    }
    else
        handleEventWithGuard(receivetime);
}

void Channel::tie(const std::shared_ptr<void> &obj)
{
    tie_ = obj;
    tied_ = true;
}

void Channel::remove()
{
    // loop_->removeChannel(this);
}

void Channel::update()
{
    // loop_->updateChannel(this);
}

void Channel::handleEventWithGuard(Timestamp receiveTime)
{
    LOG_INFO("channel handleEvent revent: %d", revents_);

    if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN))
        if (closecallback_)
            closecallback_();
    if (revents_ & EPOLLERR)
        if (errorcallback_)
            errorcallback_();
    if (revents_ & (EPOLLIN | EPOLLPRI))
        if (readcallback_)
            readcallback_(receiveTime);
    if (revents_ & EPOLLOUT)
        if (writecallback_)
            writecallback_();
}
