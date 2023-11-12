#include "Poller.hpp"

Poller::Poller(EventLoop *loop) : ownerloop_(loop)
{
}

Poller::~Poller() = default;

bool Poller::hasChannel(Channel *channel) const
{
    ChannelUnorderedMap::const_iterator it = channels_.find(channel->GetFd());
    return it != channels_.end() && it->second == channel;
}
