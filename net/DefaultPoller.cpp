#include "Poller.hpp"
#include "EpollPoller.hpp"

#include <stdlib.h>

Poller* Poller::newDefaultPoller(EventLoop *loop)
{
    return new EpollPoller(loop);
}
