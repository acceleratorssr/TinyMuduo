#ifndef _EPOLLPOLLER_HPP_
#define _EPOLLPOLLER_HPP_

#include "Poller.hpp"

#include <vector>
#include <sys/epoll.h>

class Channel;

class EpollPoller : public Poller
{
public:
    EpollPoller(EventLoop *loop);
    ~EpollPoller() override;

    Timestamp poll(int timeoutMs, ChannelList *activeChannels) override;
    void updateChannel(Channel *channel) override;
    void removeChannel(Channel *channel) override;
    
private:
    void fillActiveChannels(int numEvents, ChannelList *activeChannels) const;
    void update(int operation, Channel *channel);
    static const char* operatorTostring(int op);

    using EventList = std::vector<epoll_event>;
    EventList events_;
    static const int kInitEventListSize = 16;
    int epollfd_;
};

#endif // _EPOLLPOLLER_HPP_
