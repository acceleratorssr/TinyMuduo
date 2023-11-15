#include "EpollPoller.hpp"
#include "log.hpp"
#include "Channel.hpp"

#include <errno.h>
#include <unistd.h>
#include <string.h>

const int kNew = -1;
const int kAdded = 1;
const int kDeleted = 2;

EpollPoller::EpollPoller(EventLoop *loop)
    : Poller(loop)
    , events_(kInitEventListSize)
    , epollfd_(epoll_create1(EPOLL_CLOEXEC))
{
    if (epollfd_ < 0)
        LOG_FATAL("EpollPoller::EpollPoller -> error:%d", errno);
}

EpollPoller::~EpollPoller() 
{
    close(epollfd_);
}

Timestamp EpollPoller::poll(int timeoutMs, ChannelList *activeChannels)
{
    LOG_INFO("EpollPoller::poll -> fd total count:%lu", channels_.size());
    int numEvents = epoll_wait(epollfd_, &*events_.begin(), static_cast<int>(events_.size()), timeoutMs);
    int saveErrno = errno;
    Timestamp now(Timestamp::now());

    if (numEvents > 0)
    {
        LOG_INFO("%d events happened", numEvents);
        fillActiveChannels(numEvents, activeChannels);
        if (numEvents == events_.size())
            events_.resize(events_.size() * 2);
    }
    else if (numEvents == 0)
        LOG_DEBUG("EpollPoller::poll -> timeout!");
    else
    {
        if (saveErrno != EINTR)
        {
            errno = saveErrno;
            LOG_ERROR("EPollPoller::poll -> error");
        }
    }
    return now;
}

void EpollPoller::updateChannel(Channel *channel)
{
    const int index = channel->GetIndex();
    int fd = channel->GetFd();
    LOG_INFO("EpollPoller::updateChannel -> fd=%d events=%d index=%d", fd, channel->GetRvents(), index);

    if (index == kNew || index == kDeleted)
    {
        if (index == kNew)
            channels_[fd] = channel;

        channel->SetIndex(kAdded);
        update(EPOLL_CTL_ADD, channel);
    }
    else
    {
        if (channel->isNoneEvent())
        {
            update(EPOLL_CTL_DEL, channel);
            channel->SetIndex(kDeleted);
        }
        else
            update(EPOLL_CTL_MOD, channel);
    }
}

void EpollPoller::removeChannel(Channel *channel) 
{
    int fd = channel->GetFd();
    LOG_INFO("EpollPoller::removeChannel -> fd=%d", fd);
    channels_.erase(fd);
    int index = channel->GetIndex();
    if (index == kAdded)
        update(EPOLL_CTL_DEL, channel);
    channel->SetIndex(kNew);
}

void EpollPoller::fillActiveChannels(int numEvents, ChannelList *activeChannels) const
{
    for (int i=0; i < numEvents; ++i)
    {
        Channel *channel = static_cast<Channel*>(events_[i].data.ptr);
        channel->SetRevent(events_[i].events);
        activeChannels->push_back(channel);
    }
}

void EpollPoller::update(int operation, Channel *channel)
{
    epoll_event event;
    int fd = channel->GetFd();

    memset(&event, 0, sizeof(event));
    event.events = channel->GetRvents();
    event.data.fd = fd; 
    event.data.ptr = channel;
    LOG_TRACE("epoll_CTL -> op = %s fd = %d", operatorTostring(operation), fd);
    if (epoll_ctl(epollfd_, operation, fd, &event) < 0)
    {
        if (operation == EPOLL_CTL_DEL)
            LOG_ERROR("epoll_ctl del error:%d", errno);
        else
            LOG_FATAL("epoll_ctl add/mod error:%d", errno);
    }
}

const char *EpollPoller::operatorTostring(int op)
{
    switch (op)
    {
    case EPOLL_CTL_ADD:
        return "EPOLL_CTL_ADD";
    case EPOLL_CTL_DEL:
        return "EPOLL_CTL_ADD";
    case EPOLL_CTL_MOD:
        return "EPOLL_CTL_MOD";
    default:
        return "Unkown Operation";
    }
}
