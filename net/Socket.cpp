#include "Socket.hpp"
#include "log.hpp"

#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/tcp.h>

Socket::~Socket()
{
    close(sockfd_);
}

void Socket::bindAddress(const inetAddress &localaddr)
{
    if (bind(sockfd_, localaddr.GetSockAddr(), sizeof(sockaddr_in)) != 0)
    {
        LOG_FATAL("bind sockfd: %d fail", sockfd_);
    }
}

void Socket::listen()
{
    if (::listen(sockfd_, 1024) != 0)
    {
        LOG_FATAL("listen sockfd%d fail", sockfd_);
    }
}

int Socket::accept(inetAddress *peeraddr)
{
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    socklen_t addrsize = sizeof(addr);
    int connfd = ::accept4(sockfd_, (sockaddr *)&addr, &addrsize, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (connfd >= 0)
    {
        peeraddr->setSockAddr(addr);
    }
    return connfd;
}

void Socket::shutdownWrite()
{
    if (shutdown(sockfd_, SHUT_WR) < 0)
    {
        LOG_ERROR("sockets::shutdownWrite");
    }
}

void Socket::setTcpNoDelay(bool on)
{
    int optval = on ? 1 : 0;
    setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
}

void Socket::setReuseAddr(bool on)
{
    int optval = on ? 1 : 0;
    setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}

void Socket::setReusePort(bool on)
{
    int optval = on ? 1 : 0;
    setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
}

void Socket::setKeepAlive(bool on)
{
    int optval = on ? 1 : 0;
    setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
}

sockaddr_in getLocalAddr(int sockfd)
{
    sockaddr_in localaddr;
    memset(&localaddr, 0, sizeof(localaddr));
    socklen_t addrlen = static_cast<socklen_t>(sizeof localaddr);
    if (getsockname(sockfd, (sockaddr *)(&localaddr), &addrlen) < 0)
    {
        LOG_ERROR("socket::getLocalAddr");
    }
    return localaddr;
}

sockaddr_in getPeerAddr(int sockfd)
{
    sockaddr_in peeraddr;
    memset(&peeraddr, 0, sizeof(peeraddr));
    socklen_t addrlen = static_cast<socklen_t>(sizeof peeraddr);
    if (getsockname(sockfd, (sockaddr *)(&peeraddr), &addrlen) < 0)
    {
        LOG_ERROR("socket::getPeerAddr");
    }
    return peeraddr;
}
