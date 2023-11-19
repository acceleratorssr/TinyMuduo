#include "Acceptor.hpp"
#include "log.hpp"
#include "inetAddress.hpp"

#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>

static int createNonblocking()
{
    int sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    if (sockfd == -1)
        LOG_FATAL("%s:%s:%d sockfd::createNonblocking error: %d", __FILE__, __FUNCTION__, __LINE__, errno);
    return sockfd;
}

Acceptor::Acceptor(EventLoop *loop, const inetAddress &listenAddr, bool reuseport)
    : loop_(loop),
      acceptSocket_(createNonblocking()),
      acceptChannel_(loop, acceptSocket_.GetFd()),
      listenning_(false)
{
    acceptSocket_.setReuseAddr(true);
    acceptSocket_.bindAddress(listenAddr);
    acceptSocket_.setReusePort(true);
    acceptChannel_.setReadCallback(std::bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor()
{
    acceptChannel_.DisableAll();
    acceptChannel_.remove();
}

void Acceptor::listen()
{
    listenning_=true;
    acceptSocket_.listen();
    acceptChannel_.EnableReading();
}

void Acceptor::handleRead()
{
    inetAddress peerAddr;
    int connfd = acceptSocket_.accept(&peerAddr);
    if(connfd >= 0)
    {
        if(newconnectioncallback_)
            newconnectioncallback_(connfd, peerAddr);
        else
            close(connfd);
    }
    else
    {
        LOG_ERROR("%s:%s:%d accept error: %d", __FILE__, __FUNCTION__, __LINE__, errno);
        if(errno == EMFILE)
            LOG_ERROR("%s:%s:%d Too many open files", __FILE__, __FUNCTION__, __LINE__);
    }
}
