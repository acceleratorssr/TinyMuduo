#ifndef _POLLER_HPP_
#define _POLLER_HPP_

#include "noncopyable.hpp"
#include "Timestamp.hpp"
#include "Channel.hpp"

#include <vector>
#include <unordered_map>

class EventLoop;

class Poller : noncopyable
{
public:
    Poller(EventLoop *loop);
    virtual ~Poller();
    using ChannelList = std::vector<Channel *>;

    virtual Timestamp poll(int TimeoutMs, ChannelList *activeChannels) = 0;
    virtual void updateChannel(Channel *channel) = 0;
    virtual void removeChannel(Channel *channel) = 0;

    bool hasChannel(Channel *channel) const;

    static Poller* newDefaultPoller(EventLoop* loop);
    
protected:
    using ChannelUnorderedMap = std::unordered_map<int, Channel *>;
    ChannelUnorderedMap channels_;

private:
    EventLoop *ownerloop_;
};

#endif // _POLLER_HPP_
